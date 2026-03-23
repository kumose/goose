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

#include <goose/function/table/system_functions.h>

#include <goose/catalog/catalog.h>
#include <goose/catalog/catalog_entry/collate_catalog_entry.h>
#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/common/exception.h>

namespace goose {
    struct PragmaCollateData : public GlobalTableFunctionState {
        PragmaCollateData() : offset(0) {
        }

        vector<string> entries;
        idx_t offset;
    };

    static unique_ptr<FunctionData> PragmaCollateBind(ClientContext &context, TableFunctionBindInput &input,
                                                      vector<LogicalType> &return_types, vector<string> &names) {
        names.emplace_back("collname");
        return_types.emplace_back(LogicalType::VARCHAR);

        return nullptr;
    }

    unique_ptr<GlobalTableFunctionState> PragmaCollateInit(ClientContext &context, TableFunctionInitInput &input) {
        auto result = make_uniq<PragmaCollateData>();

        auto schemas = Catalog::GetAllSchemas(context);
        for (auto schema: schemas) {
            schema.get().Scan(context, CatalogType::COLLATION_ENTRY,
                              [&](CatalogEntry &entry) { result->entries.push_back(entry.name); });
        }
        return result;
    }

    static void PragmaCollateFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &data = data_p.global_state->Cast<PragmaCollateData>();
        if (data.offset >= data.entries.size()) {
            // finished returning values
            return;
        }
        idx_t next = MinValue<idx_t>(data.offset + STANDARD_VECTOR_SIZE, data.entries.size());
        output.SetCardinality(next - data.offset);
        for (idx_t i = data.offset; i < next; i++) {
            auto index = i - data.offset;
            output.SetValue(0, index, Value(data.entries[i]));
        }

        data.offset = next;
    }

    void PragmaCollations::RegisterFunction(BuiltinFunctions &set) {
        set.AddFunction(
            TableFunction("pragma_collations", {}, PragmaCollateFunction, PragmaCollateBind, PragmaCollateInit));
    }
} // namespace goose
