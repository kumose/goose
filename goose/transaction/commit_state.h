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

#include <goose/transaction/undo_buffer.h>
#include <goose/common/vector_size.h>
#include <goose/common/enums/index_removal_type.h>
#include <goose/main/client_context.h>

namespace goose {
    class CatalogEntry;
    class DataChunk;
    class GooseTransaction;
    class WriteAheadLog;
    class ClientContext;

    struct DataTableInfo;
    class DataTable;
    struct DeleteInfo;
    struct UpdateInfo;

    enum class CommitMode { COMMIT, REVERT_COMMIT };

    struct IndexDataRemover {
    public:
        explicit IndexDataRemover(GooseTransaction &transaction, QueryContext context, IndexRemovalType removal_type);

        void PushDelete(DeleteInfo &info);

        void Verify();

    private:
        void Flush(DataTable &table, row_t *row_numbers, idx_t count);

    private:
        GooseTransaction &transaction;
        // data for index cleanup
        QueryContext context;
        //! While committing, we remove data from any indexes that was deleted
        IndexRemovalType removal_type;
        DataChunk chunk;
        //! Debug mode only - list of indexes to verify
        reference_map_t<DataTable, shared_ptr<DataTableInfo> > verify_indexes;
    };

    class CommitState {
    public:
        explicit CommitState(GooseTransaction &transaction, transaction_t commit_id,
                             ActiveTransactionState transaction_state, CommitMode commit_mode);

    public:
        void CommitEntry(UndoFlags type, data_ptr_t data);

        void RevertCommit(UndoFlags type, data_ptr_t data);

        void Flush();

        void Verify();

        static IndexRemovalType GetIndexRemovalType(ActiveTransactionState transaction_state, CommitMode commit_mode);

    private:
        void CommitEntryDrop(CatalogEntry &entry, data_ptr_t extra_data);

        void CommitDelete(DeleteInfo &info);

    private:
        GooseTransaction &transaction;
        transaction_t commit_id;
        IndexDataRemover index_data_remover;
    };
} // namespace goose
