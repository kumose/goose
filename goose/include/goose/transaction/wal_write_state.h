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

namespace goose {
    class CatalogEntry;
    class DataChunk;
    class GooseTransaction;
    class WriteAheadLog;
    class ClientContext;

    struct DataTableInfo;
    struct DeleteInfo;
    struct UpdateInfo;

    class WALWriteState {
    public:
        explicit WALWriteState(GooseTransaction &transaction, WriteAheadLog &log,
                               optional_ptr<StorageCommitState> commit_state);

    public:
        void CommitEntry(UndoFlags type, data_ptr_t data);

    private:
        void SwitchTable(DataTableInfo &table, UndoFlags new_op);

        void WriteCatalogEntry(CatalogEntry &entry, data_ptr_t extra_data);

        void WriteDelete(DeleteInfo &info);

        void WriteUpdate(UpdateInfo &info);

    private:
        GooseTransaction &transaction;
        WriteAheadLog &log;
        optional_ptr<StorageCommitState> commit_state;

        optional_ptr<DataTableInfo> current_table_info;

        unique_ptr<DataChunk> delete_chunk;
        unique_ptr<DataChunk> update_chunk;
    };
} // namespace goose
