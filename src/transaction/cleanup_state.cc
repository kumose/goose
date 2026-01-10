#include <goose/transaction/cleanup_state.h>
#include <goose/transaction/delete_info.h>
#include <goose/transaction/update_info.h>
#include <goose/transaction/append_info.h>

#include <goose/storage/data_table.h>

#include <goose/catalog/catalog.h>
#include <goose/catalog/dependency_manager.h>
#include <goose/storage/table/chunk_info.h>
#include <goose/storage/table/update_segment.h>
#include <goose/storage/table/row_version_manager.h>
#include <goose/transaction/commit_state.h>

namespace goose {

CleanupState::CleanupState(GooseTransaction &transaction, transaction_t lowest_active_transaction,
                           ActiveTransactionState transaction_state)
    : lowest_active_transaction(lowest_active_transaction), transaction_state(transaction_state),
      index_data_remover(transaction, QueryContext(), IndexRemovalType::DELETED_ROWS_IN_USE) {
}

void CleanupState::CleanupEntry(UndoFlags type, data_ptr_t data) {
	switch (type) {
	case UndoFlags::CATALOG_ENTRY: {
		auto catalog_entry = Load<CatalogEntry *>(data);
		D_ASSERT(catalog_entry);
		auto &entry = *catalog_entry;
		D_ASSERT(entry.set);
		entry.set->CleanupEntry(entry);
		break;
	}
	case UndoFlags::INSERT_TUPLE: {
		auto info = reinterpret_cast<AppendInfo *>(data);
		// mark the tuples as committed
		info->table->CleanupAppend(lowest_active_transaction, info->start_row, info->count);
		break;
	}
	case UndoFlags::DELETE_TUPLE: {
		auto info = reinterpret_cast<DeleteInfo *>(data);
		CleanupDelete(*info);
		break;
	}
	case UndoFlags::UPDATE_TUPLE: {
		auto info = reinterpret_cast<UpdateInfo *>(data);
		CleanupUpdate(*info);
		break;
	}
	default:
		break;
	}
}

void CleanupState::CleanupUpdate(UpdateInfo &info) {
	// remove the update info from the update chain
	info.segment->CleanupUpdate(info);
}

void CleanupState::CleanupDelete(DeleteInfo &info) {
	if (transaction_state == ActiveTransactionState::NO_OTHER_TRANSACTIONS) {
		// if there are no active transactions we don't need to do any clean-up, as we haven't written to
		// deleted_rows_in_use
		return;
	}
	index_data_remover.PushDelete(info);
}

} // namespace goose
