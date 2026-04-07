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
#include <goose/storage/buffer_manager.h>

namespace goose {
    struct GooseTemporaryFilesData : public GlobalTableFunctionState {
        GooseTemporaryFilesData() : offset(0) {
        }

        vector<TemporaryFileInformation> entries;
        idx_t offset;
    };

    static unique_ptr<FunctionData> GooseTemporaryFilesBind(ClientContext &context, TableFunctionBindInput &input,
                                                            vector<LogicalType> &return_types, vector<string> &names) {
        names.emplace_back("path");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("size");
        return_types.emplace_back(LogicalType::BIGINT);

        return nullptr;
    }

    unique_ptr<GlobalTableFunctionState>
    GooseTemporaryFilesInit(ClientContext &context, TableFunctionInitInput &input) {
        auto result = make_uniq<GooseTemporaryFilesData>();

        result->entries = BufferManager::GetBufferManager(context).GetTemporaryFiles();
        return std::move(result);
    }

    void GooseTemporaryFilesFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &data = data_p.global_state->Cast<GooseTemporaryFilesData>();
        if (data.offset >= data.entries.size()) {
            // finished returning values
            return;
        }
        // start returning values
        // either fill up the chunk or return all the remaining columns
        idx_t count = 0;
        while (data.offset < data.entries.size() && count < STANDARD_VECTOR_SIZE) {
            auto &entry = data.entries[data.offset++];
            // return values:
            idx_t col = 0;
            // database_name, VARCHAR
            output.SetValue(col++, count, entry.path);
            // database_oid, BIGINT
            output.SetValue(col++, count, Value::BIGINT(NumericCast<int64_t>(entry.size)));
            count++;
        }
        output.SetCardinality(count);
    }

    void GooseTemporaryFilesFun::RegisterFunction(BuiltinFunctions &set) {
        set.AddFunction(TableFunction("goose_temporary_files", {}, GooseTemporaryFilesFunction,
                                      GooseTemporaryFilesBind,
                                      GooseTemporaryFilesInit));
    }
} // namespace goose
