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
#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/catalog/catalog_entry/type_catalog_entry.h>
#include <goose/common/enum_util.h>
#include <goose/common/exception.h>
#include <goose/main/client_context.h>
#include <goose/main/client_config.h>
#include <goose/main/client_data.h>

namespace goose {
    struct VariableData {
        string name;
        Value value;
    };

    struct GooseVariablesData : public GlobalTableFunctionState {
        GooseVariablesData() : offset(0) {
        }

        vector<VariableData> variables;
        idx_t offset;
    };

    static unique_ptr<FunctionData> GooseVariablesBind(ClientContext &context, TableFunctionBindInput &input,
                                                       vector<LogicalType> &return_types, vector<string> &names) {
        names.emplace_back("name");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("value");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("type");
        return_types.emplace_back(LogicalType::VARCHAR);

        return nullptr;
    }

    unique_ptr<GlobalTableFunctionState> GooseVariablesInit(ClientContext &context, TableFunctionInitInput &input) {
        auto result = make_uniq<GooseVariablesData>();
        auto &config = ClientConfig::GetConfig(context);

        for (auto &entry: config.user_variables) {
            VariableData data;
            data.name = entry.first;
            data.value = entry.second;
            result->variables.push_back(std::move(data));
        }
        return std::move(result);
    }

    void GooseVariablesFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &data = data_p.global_state->Cast<GooseVariablesData>();
        if (data.offset >= data.variables.size()) {
            // finished returning values
            return;
        }
        // start returning values
        // either fill up the chunk or return all the remaining columns
        idx_t count = 0;
        while (data.offset < data.variables.size() && count < STANDARD_VECTOR_SIZE) {
            auto &variable_entry = data.variables[data.offset++];

            // return values:
            idx_t col = 0;
            // name, VARCHAR
            output.SetValue(col++, count, Value(variable_entry.name));
            // value, BIGINT
            output.SetValue(col++, count, Value(variable_entry.value.ToString()));
            // type, VARCHAR
            output.SetValue(col, count, Value(variable_entry.value.type().ToString()));
            count++;
        }
        output.SetCardinality(count);
    }

    void GooseVariablesFun::RegisterFunction(BuiltinFunctions &set) {
        set.AddFunction(
            TableFunction("goose_variables", {}, GooseVariablesFunction, GooseVariablesBind, GooseVariablesInit));
    }
} // namespace goose
