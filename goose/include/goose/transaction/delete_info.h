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

namespace goose {
    class DataTable;
    class RowVersionManager;

    struct DeleteInfo {
        DataTable *table;
        RowVersionManager *version_info;
        idx_t vector_idx;
        idx_t count;
        idx_t base_row;
        //! Whether or not row ids are consecutive (0, 1, 2, ..., count).
        //! If this is true no rows are stored and `rows` should not be accessed.
        bool is_consecutive;

        uint16_t *GetRows() {
            if (is_consecutive) {
                throw InternalException("DeleteInfo is consecutive - rows are not accessible");
            }
            return rows;
        }

        const uint16_t *GetRows() const {
            if (is_consecutive) {
                throw InternalException("DeleteInfo is consecutive - rows are not accessible");
            }
            return rows;
        }

    private:
        //! The per-vector row identifiers (actual row id is base_row + rows[x])
        uint16_t rows[1];
    };
} // namespace goose
