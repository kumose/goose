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
//

#include <goose/transaction/rollback_state.h>
#include <goose/transaction/append_info.h>
#include <goose/transaction/delete_info.h>
#include <goose/transaction/update_info.h>

#include <goose/storage/table/chunk_info.h>

#include <goose/catalog/catalog_entry.h>
#include <goose/catalog/catalog_set.h>
#include <goose/storage/data_table.h>
#include <goose/storage/table/update_segment.h>
#include <goose/storage/table/row_version_manager.h>
#include <goose/main/attached_database.h>

namespace goose {
    RollbackState::RollbackState(GooseTransaction &transaction_p) : transaction(transaction_p) {
    }

    void RollbackState::RollbackEntry(UndoFlags type, data_ptr_t data) {
        switch (type) {
            case UndoFlags::CATALOG_ENTRY: {
                // Load and undo the catalog entry.
                auto catalog_entry = Load<CatalogEntry *>(data);
                D_ASSERT(catalog_entry->set);
                catalog_entry->set->Undo(*catalog_entry);
                break;
            }
            case UndoFlags::INSERT_TUPLE: {
                auto info = reinterpret_cast<AppendInfo *>(data);
                // revert the append in the base table
                info->table->RevertAppend(transaction, info->start_row, info->count);
                break;
            }
            case UndoFlags::DELETE_TUPLE: {
                auto info = reinterpret_cast<DeleteInfo *>(data);
                // reset the deleted flag on rollback
                info->version_info->CommitDelete(info->vector_idx, NOT_DELETED_ID, *info);
                break;
            }
            case UndoFlags::UPDATE_TUPLE: {
                auto info = reinterpret_cast<UpdateInfo *>(data);
                info->segment->RollbackUpdate(*info);
                break;
            }
            case UndoFlags::ATTACHED_DATABASE: {
                auto db = Load<AttachedDatabase *>(data);
                auto &db_manager = DatabaseManager::Get(db->GetDatabase());
                db_manager.DetachInternal(db->name);
                break;
            }
            case UndoFlags::SEQUENCE_VALUE:
                break;
            default: // LCOV_EXCL_START
                D_ASSERT(type == UndoFlags::EMPTY_ENTRY);
                break;
        } // LCOV_EXCL_STOP
    }
} // namespace goose
