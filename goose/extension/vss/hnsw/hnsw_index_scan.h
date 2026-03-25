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

#pragma once

#include <goose/common/helper.h>
#include <goose/common/typedefs.h>
#include <goose/common/unique_ptr.h>
#include <goose/function/function.h>
#include <goose/function/table_function.h>
#include <goose/function/table/table_scan.h>

namespace goose {
    class Index;

    // This is created by the optimizer rule
    struct HNSWIndexScanBindData final : public TableScanBindData {
        explicit HNSWIndexScanBindData(TableCatalogEntry &table, Index &index, idx_t limit,
                                       unsafe_unique_array<float> query)
            : TableScanBindData(table), index(index), limit(limit), query(std::move(query)) {
        }

        //! The index to use
        Index &index;

        //! The limit of the scan
        idx_t limit;

        //! The query vector
        unsafe_unique_array<float> query;

    public:
        bool Equals(const FunctionData &other_p) const override {
            auto &other = other_p.Cast<HNSWIndexScanBindData>();
            return &other.table == &table;
        }
    };

    struct HNSWIndexScanFunction {
        static TableFunction GetFunction();
    };
} // namespace goose
