// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <goose/transaction/transaction_manager.h>
#include <goose/storage/storage_lock.h>
#include <goose/common/enums/checkpoint_type.h>
#include <goose/common/queue.h>

namespace goose {
    class GooseTransactionManager;
    class GooseTransaction;
    struct UndoBufferProperties;

    //! CleanupInfo collects transactions awaiting cleanup.
    //! This ensures we can clean up after releasing the transaction lock.
    struct GooseCleanupInfo {
        //! All transactions in a cleanup info share the same lowest_start_time.
        transaction_t lowest_start_time;
        vector<unique_ptr<GooseTransaction> > transactions;

        void Cleanup() noexcept;

        bool ScheduleCleanup() noexcept;
    };

    struct ActiveCheckpointWrapper {
        explicit ActiveCheckpointWrapper(GooseTransactionManager &manager);

        ~ActiveCheckpointWrapper();

        void Clear();

        GooseTransactionManager &manager;
        bool is_cleared;
    };

    //! The Transaction Manager is responsible for creating and managing
    //! transactions
    class GooseTransactionManager : public TransactionManager {
    public:
        explicit GooseTransactionManager(AttachedDatabase &db);

        ~GooseTransactionManager() override;

    public:
        static GooseTransactionManager &Get(AttachedDatabase &db);

        //! Start a new transaction
        Transaction &StartTransaction(ClientContext &context) override;

        //! Commit the given transaction
        ErrorData CommitTransaction(ClientContext &context, Transaction &transaction) override;

        //! Rollback the given transaction
        void RollbackTransaction(Transaction &transaction) override;

        void Checkpoint(ClientContext &context, bool force = false) override;

        transaction_t LowestActiveId() const {
            return lowest_active_id;
        }

        transaction_t LowestActiveStart() const {
            return lowest_active_start;
        }

        transaction_t GetLastCommit() const {
            return last_commit;
        }

        transaction_t GetActiveCheckpoint() const {
            return active_checkpoint;
        }

        transaction_t GetNewCheckpointId();

        void ResetCheckpointId();

        bool IsGooseTransactionManager() override {
            return true;
        }

        //! Obtains a shared lock to the checkpoint lock
        unique_ptr<StorageLockKey> SharedCheckpointLock();

        //! Try to obtain an exclusive checkpoint lock
        unique_ptr<StorageLockKey> TryGetCheckpointLock();

        unique_ptr<StorageLockKey> TryUpgradeCheckpointLock(StorageLockKey &lock);

        unique_ptr<StorageLockKey> SharedVacuumLock();

        unique_ptr<StorageLockKey> TryGetVacuumLock();

        //! Returns the current version of the catalog (incremented whenever anything changes, not stored between restarts)
        GOOSE_API idx_t GetCatalogVersion(Transaction &transaction);

        void PushCatalogEntry(Transaction &transaction_p, CatalogEntry &entry, data_ptr_t extra_data = nullptr,
                              idx_t extra_data_size = 0);

        void PushAttach(Transaction &transaction_p, AttachedDatabase &db);

    protected:
        struct CheckpointDecision {
            explicit CheckpointDecision(string reason_p);

            explicit CheckpointDecision(CheckpointType type);

            ~CheckpointDecision();

            bool can_checkpoint;
            string reason;
            CheckpointType type;
        };

    private:
        //! Generates a new commit timestamp
        transaction_t GetCommitTimestamp();

        //! Remove the given transaction from the list of active transactions
        unique_ptr<GooseCleanupInfo> RemoveTransaction(GooseTransaction &transaction) noexcept;

        //! Remove the given transaction from the list of active transactions
        unique_ptr<GooseCleanupInfo> RemoveTransaction(GooseTransaction &transaction, bool store_transaction) noexcept;

        //! Whether or not we can checkpoint
        CheckpointDecision CanCheckpoint(GooseTransaction &transaction, unique_ptr<StorageLockKey> &checkpoint_lock,
                                         const UndoBufferProperties &properties);

        bool HasOtherTransactions(GooseTransaction &transaction);

    private:
        //! The current start timestamp used by transactions
        transaction_t current_start_timestamp;
        //! The current transaction ID used by transactions
        transaction_t current_transaction_id;
        //! The lowest active transaction id
        atomic<transaction_t> lowest_active_id;
        //! The lowest active transaction timestamp
        atomic<transaction_t> lowest_active_start;
        //! The last commit timestamp
        atomic<transaction_t> last_commit;
        //! The currently active checkpoint
        atomic<transaction_t> active_checkpoint;
        //! Set of currently running transactions
        vector<unique_ptr<GooseTransaction> > active_transactions;
        //! Set of recently committed transactions
        vector<unique_ptr<GooseTransaction> > recently_committed_transactions;
        //! The lock used for transaction operations
        mutex transaction_lock;
        //! The checkpoint lock
        StorageLock checkpoint_lock;
        //! The vacuum lock - necessary to start vacuum operations
        StorageLock vacuum_lock;
        //! Lock necessary to start transactions only - used by FORCE CHECKPOINT to prevent new transactions from starting
        mutex start_transaction_lock;

        atomic<idx_t> last_uncommitted_catalog_version = {TRANSACTION_ID_START};
        idx_t last_committed_version = 0;

        //! Only one cleanup can be active at any time.
        mutex cleanup_lock;
        //! Changes to the cleanup queue must be synchronized.
        mutex cleanup_queue_lock;
        //! Cleanups have to happen in-order.
        //! E.g., if one transaction drops a table, and another creates a table,
        //! inverting the cleanup order can result in catalog errors.
        queue<unique_ptr<GooseCleanupInfo> > cleanup_queue;

    protected:
        virtual void OnCommitCheckpointDecision(const CheckpointDecision &decision, GooseTransaction &transaction) {
        }
    };
} // namespace goose
