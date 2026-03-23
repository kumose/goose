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
#include <goose/common/types/data_chunk.h>
#include <goose/common/types-import.h>
#include <goose/main/client_context.h>
#include <goose/transaction/commit_state.h>

namespace goose {
    class DataTable;

    struct DeleteInfo;
    struct UpdateInfo;

    class CleanupState {
    public:
        explicit CleanupState(GooseTransaction &transaction, transaction_t lowest_active_transaction,
                              ActiveTransactionState transaction_state);

    public:
        void CleanupEntry(UndoFlags type, data_ptr_t data);

    private:
        //! Lowest active transaction
        transaction_t lowest_active_transaction;
        ActiveTransactionState transaction_state;
        //! While cleaning up, we remove data from any delta indexes we added data to during the commit
        IndexDataRemover index_data_remover;

    private:
        void CleanupDelete(DeleteInfo &info);

        void CleanupUpdate(UpdateInfo &info);
    };
} // namespace goose
