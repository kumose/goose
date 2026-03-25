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

#include <goose/common/constants.h>
#include <goose/common/types/vector.h>
#include <goose/common/types/validity_mask.h>
#include <goose/transaction/undo_buffer_allocator.h>
#include <goose/common/types-import.h>

namespace goose {
    class UpdateSegment;
    struct DataTableInfo;
    class DataTable;

    //! UpdateInfo is a class that represents a set of updates applied to a single vector.
    //! The UpdateInfo struct contains metadata associated with the update.
    //! After the UpdateInfo, we must ALWAYS allocate the list of tuples and the data as contiguous arrays:
    //! [UpdateInfo][TUPLES (sel_t[max])][DATA (T[max])]
    //! The required allocation size can be obtained using UpdateInfo::GetAllocSize
    struct UpdateInfo {
        //! The update segment that this update info affects
        UpdateSegment *segment;
        //! The table this was update was made on
        DataTable *table;
        //! The column index of which column we are updating
        idx_t column_index;
        //! The start index of the row group
        idx_t row_group_start;
        //! The version number
        atomic<transaction_t> version_number;
        //! The vector index within the uncompressed segment
        idx_t vector_index;
        //! The amount of updated tuples
        sel_t N; // NOLINT
        //! The maximum amount of tuples that can fit into this UpdateInfo
        sel_t max;
        //! The previous update info (or nullptr if it is the base)
        UndoBufferPointer prev;
        //! The next update info in the chain (or nullptr if it is the last)
        UndoBufferPointer next;

        //! The row ids of the tuples that have been updated. This should always be kept sorted!
        sel_t *GetTuples();

        //! The update values
        data_ptr_t GetValues();

        template<class T>
        T *GetData() {
            return reinterpret_cast<T *>(GetValues());
        }

        bool AppliesToTransaction(transaction_t start_time, transaction_t transaction_id) {
            // these tuples were either committed AFTER this transaction started or are not committed yet, use
            // tuples stored in this version
            return version_number > start_time && version_number != transaction_id;
        }

        //! Loop over the update chain and execute the specified callback on all UpdateInfo's that are relevant for that
        //! transaction in-order of newest to oldest
        template<class T>
        static void UpdatesForTransaction(UpdateInfo &current, transaction_t start_time, transaction_t transaction_id,
                                          T &&callback) {
            if (current.AppliesToTransaction(start_time, transaction_id)) {
                callback(current);
            }
            auto update_ptr = current.next;
            while (update_ptr.IsSet()) {
                auto pin = update_ptr.Pin();
                auto &info = Get(pin);
                if (info.AppliesToTransaction(start_time, transaction_id)) {
                    callback(info);
                }
                update_ptr = info.next;
            }
        }

        Value GetValue(idx_t index);

        string ToString();

        void Print();

        void Verify();

        bool HasPrev() const;

        bool HasNext() const;

        static UpdateInfo &Get(UndoBufferReference &entry);

        //! Returns the total allocation size for an UpdateInfo entry, together with space for the tuple data
        static idx_t GetAllocSize(idx_t type_size);

        //! Initialize an UpdateInfo struct that has been allocated using GetAllocSize (i.e. has extra space after it)
        static void Initialize(UpdateInfo &info, DataTable &data_table, transaction_t transaction_id,
                               idx_t row_group_start);
    };
} // namespace goose
