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


#include <goose/shell/shell_extension.h>
#include <goose/main/extension/extension_loader.h>
#include <goose/common/vector_operations/unary_executor.h>
#include <goose/main/config.h>
#include <goose/shell/shell_state.h>
#include <goose/parser/tableref/column_data_ref.h>
#include <goose/shell/shell_renderer.h>
#include <stdio.h>
#include <stdlib.h>

namespace goose {
    static void GetEnvFunction(DataChunk &args, ExpressionState &state, Vector &result) {
        UnaryExecutor::Execute<string_t, string_t>(args.data[0], result, args.size(), [&](string_t input) {
            string env_name = input.GetString();
            auto env_value = getenv(env_name.c_str());
            if (!env_value) {
                return StringVector::AddString(result, string());
            }
            return StringVector::AddString(result, env_value);
        });
    }

    static unique_ptr<FunctionData> GetEnvBind(ClientContext &context, ScalarFunction &bound_function,
                                               vector<unique_ptr<Expression> > &arguments) {
        auto &config = DBConfig::GetConfig(context);
        if (!config.options.enable_external_access) {
            throw PermissionException("getenv is disabled through configuration");
        }
        return nullptr;
    }

    unique_ptr<TableRef> ShellScanLastResult(ClientContext &context, ReplacementScanInput &input,
                                             optional_ptr<ReplacementScanData> data) {
        auto &table_name = input.table_name;
        if (table_name != "_") {
            return nullptr;
        }
        auto &state = goose_shell::ShellState::Get();
        if (!state.last_result) {
            throw BinderException("Failed to query last result \"_\": no result available");
        }
        return make_uniq<ColumnDataRef>(state.last_result->Collection(), state.last_result->names);
    }

    void ShellExtension::Load(ExtensionLoader &loader) {
        loader.SetDescription("Adds CLI-specific support and functionalities");
        loader.RegisterFunction(
            ScalarFunction("getenv", {LogicalType::VARCHAR}, LogicalType::VARCHAR, GetEnvFunction, GetEnvBind));

        auto &config = goose::DBConfig::GetConfig(loader.GetDatabaseInstance());
        config.replacement_scans.push_back(goose::ReplacementScan(goose::ShellScanLastResult));
    }

    std::string ShellExtension::Name() {
        return "shell";
    }

    std::string ShellExtension::Version() const {
        return DefaultVersion();
    }
} // namespace goose
