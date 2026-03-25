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

#include <goose/common/common.h>
#include <goose/common/enums/undo_flags.h>
#include <goose/transaction/undo_buffer_allocator.h>
#include <goose/common/enums/active_transaction_state.h>

namespace goose {
    class BufferManager;
    class GooseTransaction;
    class StorageCommitState;
    class WriteAheadLog;
    struct UndoBufferPointer;
    struct CommitInfo;

    struct UndoBufferProperties {
        idx_t estimated_size = 0;
        bool has_updates = false;
        bool has_deletes = false;
        bool has_index_deletes = false;
        bool has_catalog_changes = false;
        bool has_dropped_entries = false;
    };

    //! The undo buffer of a transaction is used to hold previous versions of tuples
    //! that might be required in the future (because of rollbacks or previous
    //! transactions accessing them)
    class UndoBuffer {
    public:
        struct IteratorState {
            BufferHandle handle;
            optional_ptr<UndoBufferEntry> current;
            data_ptr_t start;
            data_ptr_t end;
            bool started = false;
        };

    public:
        explicit UndoBuffer(GooseTransaction &transaction, ClientContext &context);

        //! Write a specified entry to the undo buffer
        UndoBufferReference CreateEntry(UndoFlags type, idx_t len);

        bool ChangesMade();

        UndoBufferProperties GetProperties();

        //! Cleanup the undo buffer
        void Cleanup(transaction_t lowest_active_transaction);

        //! Commit the changes made in the UndoBuffer: should be called on commit
        void WriteToWAL(WriteAheadLog &wal, optional_ptr<StorageCommitState> commit_state);

        //! Commit the changes made in the UndoBuffer: should be called on commit
        void Commit(UndoBuffer::IteratorState &iterator_state, CommitInfo &info);

        //! Revert committed changes made in the UndoBuffer up until the currently committed state
        void RevertCommit(UndoBuffer::IteratorState &iterator_state, transaction_t transaction_id);

        //! Rollback the changes made in this UndoBuffer: should be called on
        //! rollback
        void Rollback();

    private:
        GooseTransaction &transaction;
        UndoBufferAllocator allocator;
        ActiveTransactionState active_transaction_state = ActiveTransactionState::UNSET;

    private:
        template<class T>
        void IterateEntries(UndoBuffer::IteratorState &state, T &&callback);

        template<class T>
        void IterateEntries(UndoBuffer::IteratorState &state, UndoBuffer::IteratorState &end_state, T &&callback);

        template<class T>
        void ReverseIterateEntries(T &&callback);
    };
} // namespace goose
