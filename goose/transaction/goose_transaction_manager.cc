#include <goose/transaction/goose_transaction_manager.h>

#include <goose/main/client_data.h>

#include <goose/catalog/catalog_set.h>
#include <goose/common/exception/transaction_exception.h>
#include <goose/common/exception.h>
#include <goose/common/helper.h>
#include <goose/common/types/timestamp.h>
#include <goose/catalog/catalog.h>
#include <goose/catalog/dependency_manager.h>
#include <goose/storage/storage_manager.h>
#include <goose/transaction/goose_transaction.h>
#include <goose/main/client_context.h>
#include <goose/main/connection_manager.h>
#include <goose/main/attached_database.h>
#include <goose/main/database_manager.h>
#include <goose/transaction/meta_transaction.h>
#include <goose/main/settings.h>

namespace goose {

void GooseCleanupInfo::Cleanup() noexcept {
	for (auto &transaction : transactions) {
		if (transaction->awaiting_cleanup) {
			transaction->Cleanup(lowest_start_time);
		}
	}
}

bool GooseCleanupInfo::ScheduleCleanup() noexcept {
	return !transactions.empty();
}

GooseTransactionManager::GooseTransactionManager(AttachedDatabase &db) : TransactionManager(db) {
	// start timestamp starts at two
	current_start_timestamp = 2;
	// transaction ID starts very high:
	// it should be much higher than the current start timestamp
	// if transaction_id < start_timestamp for any set of active transactions
	// uncommitted data could be read by
	current_transaction_id = TRANSACTION_ID_START;
	lowest_active_id = TRANSACTION_ID_START;
	lowest_active_start = MAX_TRANSACTION_ID;
	active_checkpoint = MAX_TRANSACTION_ID;
	if (!db.GetCatalog().IsGooseCatalog()) {
		// Specifically the StorageManager of the GooseCatalog is relied on, with `db.GetStorageManager`
		throw InternalException("GooseTransactionManager should only be created together with a GooseCatalog");
	}
}

GooseTransactionManager::~GooseTransactionManager() {
}

GooseTransactionManager &GooseTransactionManager::Get(AttachedDatabase &db) {
	auto &transaction_manager = TransactionManager::Get(db);
	if (!transaction_manager.IsGooseTransactionManager()) {
		throw InternalException("Calling GooseTransactionManager::Get on non-Goose transaction manager");
	}
	return reinterpret_cast<GooseTransactionManager &>(transaction_manager);
}

Transaction &GooseTransactionManager::StartTransaction(ClientContext &context) {
	// obtain the transaction lock during this function
	auto &meta_transaction = MetaTransaction::Get(context);
	unique_ptr<lock_guard<mutex>> start_lock;
	if (!meta_transaction.IsReadOnly()) {
		start_lock = make_uniq<lock_guard<mutex>>(start_transaction_lock);
	}
	lock_guard<mutex> lock(transaction_lock);
	if (current_start_timestamp >= TRANSACTION_ID_START) { // LCOV_EXCL_START
		throw InternalException("Cannot start more transactions, ran out of "
		                        "transaction identifiers!");
	} // LCOV_EXCL_STOP

	// obtain the start time and transaction ID of this transaction
	transaction_t start_time = current_start_timestamp++;
	transaction_t transaction_id = current_transaction_id++;
	if (active_transactions.empty()) {
		lowest_active_start = start_time;
		lowest_active_id = transaction_id;
	}

	// create the actual transaction
	auto transaction = make_uniq<GooseTransaction>(*this, context, start_time, transaction_id, last_committed_version);
	auto &transaction_ref = *transaction;

	// store it in the set of active transactions
	active_transactions.push_back(std::move(transaction));
	return transaction_ref;
}

ActiveCheckpointWrapper::ActiveCheckpointWrapper(GooseTransactionManager &manager)
    : manager(manager), is_cleared(false) {
}

ActiveCheckpointWrapper::~ActiveCheckpointWrapper() {
	Clear();
}

void ActiveCheckpointWrapper::Clear() {
	if (is_cleared) {
		return;
	}
	is_cleared = true;
	manager.ResetCheckpointId();
}

transaction_t GooseTransactionManager::GetNewCheckpointId() {
	if (active_checkpoint != MAX_TRANSACTION_ID) {
		throw InternalException(
		    "GooseTransactionManager::GetNewCheckpointId requested a new id but active_checkpoint was already set");
	}
	auto result = last_commit.load();
	active_checkpoint = result;
	return result;
}

void GooseTransactionManager::ResetCheckpointId() {
	active_checkpoint = MAX_TRANSACTION_ID;
}

GooseTransactionManager::CheckpointDecision::CheckpointDecision(string reason_p)
    : can_checkpoint(false), reason(std::move(reason_p)) {
}

GooseTransactionManager::CheckpointDecision::CheckpointDecision(CheckpointType type) : can_checkpoint(true), type(type) {
}

GooseTransactionManager::CheckpointDecision::~CheckpointDecision() {
}

bool GooseTransactionManager::HasOtherTransactions(GooseTransaction &transaction) {
	for (auto &active_transaction : active_transactions) {
		if (!RefersToSameObject(*active_transaction, transaction)) {
			return true;
		}
	}
	return false;
}

GooseTransactionManager::CheckpointDecision
GooseTransactionManager::CanCheckpoint(GooseTransaction &transaction, unique_ptr<StorageLockKey> &lock,
                                      const UndoBufferProperties &undo_properties) {
	if (db.IsSystem()) {
		return CheckpointDecision("system transaction");
	}
	if (transaction.IsReadOnly()) {
		return CheckpointDecision("transaction is read-only");
	}
	auto &storage_manager = db.GetStorageManager();
	if (!storage_manager.IsLoaded()) {
		return CheckpointDecision("cannot checkpoint while loading");
	}
	if (!transaction.AutomaticCheckpoint(db, undo_properties)) {
		return CheckpointDecision("no reason to automatically checkpoint");
	}
	if (DBConfig::GetSetting<DebugSkipCheckpointOnCommitSetting>(db.GetDatabase())) {
		return CheckpointDecision("checkpointing on commit disabled through configuration");
	}
	// try to lock the checkpoint lock
	lock = transaction.TryGetCheckpointLock();
	if (!lock) {
		return CheckpointDecision("Failed to obtain checkpoint lock - another thread is writing/checkpointing or "
		                          "another read transaction relies on data that is not yet committed");
	}
	auto checkpoint_type = CheckpointType::FULL_CHECKPOINT;
	bool has_other_transactions = HasOtherTransactions(transaction);
	if (has_other_transactions) {
		if (undo_properties.has_updates || undo_properties.has_dropped_entries) {
			// if we have made updates/catalog changes in this transaction we cannot checkpoint
			// in the presence of other transactions
			string other_transactions;
			for (auto &active_transaction : active_transactions) {
				if (!RefersToSameObject(*active_transaction, transaction)) {
					if (!other_transactions.empty()) {
						other_transactions += ", ";
					}
					other_transactions += "[" + to_string(active_transaction->transaction_id) + "]";
				}
			}
			if (!other_transactions.empty()) {
				// there are other transactions!
				// these active transactions might need data from BEFORE this transaction
				// we might need to change our strategy here based on what changes THIS transaction has made
				if (undo_properties.has_dropped_entries) {
					// this transaction has changed the catalog - we cannot checkpoint
					return CheckpointDecision(
					    "Transaction has dropped catalog entries and there are other transactions "
					    "active\nActive transactions: " +
					    other_transactions);
				}
				// this transaction has performed updates - we cannot checkpoint
				return CheckpointDecision(
				    "Transaction has performed updates and there are other transactions active\nActive transactions: " +
				    other_transactions);
			}
		}
		// otherwise - we need to do a concurrent checkpoint
		checkpoint_type = CheckpointType::CONCURRENT_CHECKPOINT;
	}
	if (storage_manager.InMemory() && !storage_manager.CompressionIsEnabled()) {
		if (checkpoint_type == CheckpointType::CONCURRENT_CHECKPOINT) {
			return CheckpointDecision("Cannot vacuum, and compression is disabled for in-memory table");
		}
		return CheckpointDecision(CheckpointType::VACUUM_ONLY);
	}
	return CheckpointDecision(checkpoint_type);
}

void GooseTransactionManager::Checkpoint(ClientContext &context, bool force) {
	auto &storage_manager = db.GetStorageManager();
	auto current = Transaction::TryGet(context, db);
	if (current) {
		if (force) {
			throw TransactionException(
			    "Cannot FORCE CHECKPOINT: the current transaction has been started for this database");
		} else {
			auto &transaction = current->Cast<GooseTransaction>();
			if (transaction.ChangesMade()) {
				throw TransactionException("Cannot CHECKPOINT: the current transaction has transaction local changes");
			}
		}
	}

	unique_ptr<StorageLockKey> lock;
	if (!force) {
		// not a force checkpoint
		// try to get the checkpoint lock
		lock = checkpoint_lock.TryGetExclusiveLock();
		if (!lock) {
			// we could not manage to get the lock - cancel
			throw TransactionException("Cannot CHECKPOINT: there are other write transactions active. Try using FORCE "
			                           "CHECKPOINT to wait until all active transactions are finished");
		}

	} else {
		// force checkpoint - wait to get an exclusive lock
		// grab the start_transaction_lock to prevent new transactions from starting
		lock_guard<mutex> start_lock(start_transaction_lock);
		// wait until any active transactions are finished
		while (!lock) {
			if (context.interrupted) {
				throw InterruptException();
			}
			lock = checkpoint_lock.TryGetExclusiveLock();
		}
	}
	CheckpointOptions options;
	if (GetLastCommit() > LowestActiveStart()) {
		// we cannot do a full checkpoint if any transaction needs to read old data
		options.type = CheckpointType::CONCURRENT_CHECKPOINT;
	}

	storage_manager.CreateCheckpoint(context, options);
}

unique_ptr<StorageLockKey> GooseTransactionManager::SharedCheckpointLock() {
	return checkpoint_lock.GetSharedLock();
}

unique_ptr<StorageLockKey> GooseTransactionManager::TryUpgradeCheckpointLock(StorageLockKey &lock) {
	return checkpoint_lock.TryUpgradeCheckpointLock(lock);
}

unique_ptr<StorageLockKey> GooseTransactionManager::TryGetCheckpointLock() {
	return checkpoint_lock.TryGetExclusiveLock();
}

unique_ptr<StorageLockKey> GooseTransactionManager::SharedVacuumLock() {
	return vacuum_lock.GetSharedLock();
}

unique_ptr<StorageLockKey> GooseTransactionManager::TryGetVacuumLock() {
	return vacuum_lock.TryGetExclusiveLock();
}

transaction_t GooseTransactionManager::GetCommitTimestamp() {
	return current_start_timestamp++;
}

ErrorData GooseTransactionManager::CommitTransaction(ClientContext &context, Transaction &transaction_p) {
	auto &transaction = transaction_p.Cast<GooseTransaction>();
	unique_lock<mutex> t_lock(transaction_lock);
	if (!db.IsSystem() && !db.IsTemporary()) {
		if (transaction.ChangesMade()) {
			if (transaction.IsReadOnly()) {
				throw InternalException("Attempting to commit a transaction that is read-only but has made changes - "
				                        "this should not be possible");
			}
		}
	}

	// check if we can checkpoint
	unique_ptr<StorageLockKey> lock;
	auto undo_properties = transaction.GetUndoProperties();
	auto checkpoint_decision = CanCheckpoint(transaction, lock, undo_properties);
	ErrorData error;
	unique_ptr<lock_guard<mutex>> held_wal_lock;
	unique_ptr<StorageCommitState> commit_state;
	if (!checkpoint_decision.can_checkpoint && transaction.ShouldWriteToWAL(db)) {
		auto &storage_manager = db.GetStorageManager().Cast<SingleFileStorageManager>();
		// if we are committing changes and we are not checkpointing, we need to write to the WAL
		// since WAL writes can take a long time - we grab the WAL lock here and unlock the transaction lock
		// read-only transactions can bypass this branch and start/commit while the WAL write is happening
		// unlock the transaction lock while we write to the WAL
		t_lock.unlock();
		// grab the WAL lock and hold it until the entire commit is finished
		held_wal_lock = storage_manager.GetWALLock();

		// Commit the changes to the WAL.
		if (db.GetRecoveryMode() == RecoveryMode::DEFAULT) {
			error = transaction.WriteToWAL(context, db, commit_state);
		}

		// after we finish writing to the WAL we grab the transaction lock again
		t_lock.lock();
	}
	// in-memory databases don't have a WAL - we estimate how large their changeset is based on the undo properties
	if (!db.IsSystem()) {
		auto &storage_manager = db.GetStorageManager();
		if (storage_manager.InMemory() || db.GetRecoveryMode() == RecoveryMode::NO_WAL_WRITES) {
			storage_manager.AddWALSize(undo_properties.estimated_size);
		}
	}
	// obtain a commit id for the transaction
	CommitInfo info;
	info.commit_id = GetCommitTimestamp();

	// commit the UndoBuffer of the transaction
	if (!error.HasError()) {
		if (HasOtherTransactions(transaction)) {
			info.active_transactions = ActiveTransactionState::OTHER_TRANSACTIONS;
		} else {
			info.active_transactions = ActiveTransactionState::NO_OTHER_TRANSACTIONS;
		}
		error = transaction.Commit(db, info, std::move(commit_state));
	}

	if (error.HasError()) {
		GOOSE_LOG(context, TransactionLogType, db, "Rollback (after failed commit)", info.commit_id);

		// COMMIT not successful: ROLLBACK.
		checkpoint_decision = CheckpointDecision(error.Message());
		transaction.commit_id = 0;

		auto rollback_error = transaction.Rollback();
		if (rollback_error.HasError()) {
			throw FatalException(
			    "Failed to rollback transaction. Cannot continue operation.\nOriginal Error: %s\nRollback Error: %s",
			    error.Message(), rollback_error.Message());
		}
	} else {
		GOOSE_LOG(context, TransactionLogType, db, "Commit", info.commit_id);
		last_commit = info.commit_id;

		// check if catalog changes were made
		if (transaction.catalog_version >= TRANSACTION_ID_START) {
			transaction.catalog_version = ++last_committed_version;
		}
	}
	OnCommitCheckpointDecision(checkpoint_decision, transaction);

	if (!checkpoint_decision.can_checkpoint && lock) {
		// we won't checkpoint after all: unlock the checkpoint lock again
		lock.reset();
	}

	// commit successful: remove the transaction id from the list of active transactions
	// potentially resulting in garbage collection
	bool store_transaction = undo_properties.has_updates || undo_properties.has_index_deletes ||
	                         undo_properties.has_catalog_changes || error.HasError();

	// Remove the transaction from the list of active transactions and gather cleanup information.
	auto cleanup_info = RemoveTransaction(transaction, store_transaction);
	if (cleanup_info->ScheduleCleanup()) {
		lock_guard<mutex> q_lock(cleanup_queue_lock);
		cleanup_queue.emplace(std::move(cleanup_info));
	}

	// We do not need to hold the transaction lock during cleanup of transactions,
	// as they (1) have been removed, or (2) enter cleanup_info.
	t_lock.unlock();
	held_wal_lock.reset();

	{
		lock_guard<mutex> c_lock(cleanup_lock);
		unique_ptr<GooseCleanupInfo> top_cleanup_info;
		{
			lock_guard<mutex> q_lock(cleanup_queue_lock);
			if (!cleanup_queue.empty()) {
				top_cleanup_info = std::move(cleanup_queue.front());
				cleanup_queue.pop();
			}
		}
		if (top_cleanup_info) {
			top_cleanup_info->Cleanup();
		}
	}

	// now perform a checkpoint if (1) we are able to checkpoint, and (2) the WAL has reached sufficient size to
	// checkpoint
	if (checkpoint_decision.can_checkpoint) {
		D_ASSERT(lock);
		// we can unlock the transaction lock while checkpointing
		// checkpoint the database to disk
		CheckpointOptions options;
		options.action = CheckpointAction::ALWAYS_CHECKPOINT;
		options.type = checkpoint_decision.type;
		auto &storage_manager = db.GetStorageManager();
		try {
			storage_manager.CreateCheckpoint(context, options);
		} catch (std::exception &ex) {
			error.Merge(ErrorData(ex));
		}
	}

	return error;
}

void GooseTransactionManager::RollbackTransaction(Transaction &transaction_p) {
	auto &transaction = transaction_p.Cast<GooseTransaction>();

	GOOSE_LOG(db.GetDatabase(), TransactionLogType, db, "Rollback", transaction.transaction_id);

	ErrorData error;
	{
		// Obtain the transaction lock and roll back.
		lock_guard<mutex> t_lock(transaction_lock);
		error = transaction.Rollback();

		// Remove the transaction from the list of active transactions and gather cleanup information.
		auto cleanup_info = RemoveTransaction(transaction);
		if (cleanup_info->ScheduleCleanup()) {
			lock_guard<mutex> q_lock(cleanup_queue_lock);
			cleanup_queue.emplace(std::move(cleanup_info));
		}
	}

	{
		lock_guard<mutex> c_lock(cleanup_lock);
		unique_ptr<GooseCleanupInfo> top_cleanup_info;
		{
			lock_guard<mutex> q_lock(cleanup_queue_lock);
			if (!cleanup_queue.empty()) {
				top_cleanup_info = std::move(cleanup_queue.front());
				cleanup_queue.pop();
			}
		}
		if (top_cleanup_info) {
			top_cleanup_info->Cleanup();
		}
	}

	if (error.HasError()) {
		throw FatalException("Failed to rollback transaction. Cannot continue operation.\nError: %s", error.Message());
	}
}

unique_ptr<GooseCleanupInfo> GooseTransactionManager::RemoveTransaction(GooseTransaction &transaction) noexcept {
	return RemoveTransaction(transaction, transaction.ChangesMade());
}

unique_ptr<GooseCleanupInfo> GooseTransactionManager::RemoveTransaction(GooseTransaction &transaction,
                                                                      bool store_transaction) noexcept {
	auto cleanup_info = make_uniq<GooseCleanupInfo>();

	// Find the transaction in the active transactions,
	// as well as the lowest start time, transaction id, and active query.
	idx_t t_index = active_transactions.size();
	auto lowest_start_time = TRANSACTION_ID_START;
	auto lowest_transaction_id = MAX_TRANSACTION_ID;
	auto active_checkpoint_id = active_checkpoint.load();
	for (idx_t i = 0; i < active_transactions.size(); i++) {
		if (active_transactions[i].get() == &transaction) {
			t_index = i;
			continue;
		}
		lowest_start_time = MinValue(lowest_start_time, active_transactions[i]->start_time);
		lowest_transaction_id = MinValue(lowest_transaction_id, active_transactions[i]->transaction_id);
	}
	if (active_checkpoint_id != MAX_TRANSACTION_ID && active_checkpoint_id < lowest_start_time) {
		lowest_start_time = active_checkpoint_id;
	}
	lowest_active_start = lowest_start_time;
	lowest_active_id = lowest_transaction_id;
	D_ASSERT(t_index != active_transactions.size());

	// Decide if we need to store the transaction, or if we can schedule it for cleanup.
	auto current_transaction = std::move(active_transactions[t_index]);
	if (store_transaction) {
		// If the transaction made any changes, we need to keep it around.
		if (transaction.commit_id != 0) {
			// The transaction was committed.
			// We add it to the list of recently committed transactions.
			recently_committed_transactions.push_back(std::move(current_transaction));
		} else {
			// The transaction was aborted.
			cleanup_info->transactions.push_back(std::move(current_transaction));
		}
	} else if (transaction.ChangesMade()) {
		// We do not need to store the transaction, directly schedule it for cleanup.
		current_transaction->awaiting_cleanup = true;
		cleanup_info->transactions.push_back(std::move(current_transaction));
	}
	cleanup_info->lowest_start_time = lowest_start_time;

	// Remove the transaction from the list of active transactions.
	active_transactions.unsafe_erase_at(t_index);

	// Traverse the recently_committed transactions to see if we can move any
	// to the list of transactions awaiting GC.
	idx_t i = 0;
	for (; i < recently_committed_transactions.size(); i++) {
		D_ASSERT(recently_committed_transactions[i]);
		if (recently_committed_transactions[i]->commit_id >= lowest_start_time) {
			// recently_committed_transactions is ordered on commit_id.
			// Thus, if the current commit_id is greater than
			// lowest_start_time, any subsequent commit IDs are also greater.
			break;
		}

		recently_committed_transactions[i]->awaiting_cleanup = true;
		cleanup_info->transactions.push_back(std::move(recently_committed_transactions[i]));
	}

	if (i > 0) {
		// We moved these transactions to the list of transactions awaiting GC.
		auto start = recently_committed_transactions.begin();
		auto end = recently_committed_transactions.begin() + static_cast<int64_t>(i);
		recently_committed_transactions.erase(start, end);
	}

	return cleanup_info;
}

idx_t GooseTransactionManager::GetCatalogVersion(Transaction &transaction_p) {
	auto &transaction = transaction_p.Cast<GooseTransaction>();
	return transaction.catalog_version;
}

void GooseTransactionManager::PushCatalogEntry(Transaction &transaction_p, goose::CatalogEntry &entry,
                                              goose::data_ptr_t extra_data, goose::idx_t extra_data_size) {
	auto &transaction = transaction_p.Cast<GooseTransaction>();
	if (!db.IsSystem() && !db.IsTemporary() && transaction.IsReadOnly()) {
		throw InternalException("Attempting to do catalog changes on a transaction that is read-only - "
		                        "this should not be possible");
	}
	transaction.catalog_version = ++last_uncommitted_catalog_version;
	transaction.PushCatalogEntry(entry, extra_data, extra_data_size);
}

void GooseTransactionManager::PushAttach(Transaction &transaction_p, AttachedDatabase &attached_db) {
	auto &transaction = transaction_p.Cast<GooseTransaction>();
	if (!db.IsSystem()) {
		throw InternalException("Can only ATTACH in the system catalog");
	}
	transaction.catalog_version = ++last_uncommitted_catalog_version;
	transaction.PushAttach(attached_db);
}

} // namespace goose
