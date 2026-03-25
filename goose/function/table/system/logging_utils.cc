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

#include <goose/catalog/catalog_entry/collate_catalog_entry.h>
#include <goose/common/exception.h>
#include <goose/function/table_function.h>
#include <goose/logging/log_manager.h>
#include <goose/logging/logging.h>
#include <goose/main/client_context.h>
#include <goose/main/database.h>

namespace goose {
    class EnableLoggingBindData : public TableFunctionData {
    public:
        EnableLoggingBindData() {
        }

        case_insensitive_map_t<Value> storage_config;
        LogConfig config;
        vector<string> log_types_to_set;
    };

    static void EnableLogging(ClientContext &context, TableFunctionInput &data, DataChunk &output) {
        auto bind_data = data.bind_data->Cast<EnableLoggingBindData>();

        GOOSE_LOG_WARNING(
            context, "The logging settings have been changed so you may lose warnings printed in the CLI.\n"
            "To continue printing warnings to the console, set storage='shell_log_storage'.\n"
            "For more info see https://duckdb.org/docs/stable/operations_manual/logging/overview.")

        auto &log_manager = context.db->GetLogManager();

        // Apply the config generated from the input
        log_manager.SetConfig(*context.db, bind_data.config);

        if (bind_data.log_types_to_set.empty()) {
            log_manager.SetEnableLogging(true);
            log_manager.SetLogMode(LogMode::LEVEL_ONLY);
        } else {
            log_manager.SetEnableStructuredLoggers(bind_data.log_types_to_set);
        }

        if (!bind_data.storage_config.empty()) {
            log_manager.UpdateLogStorageConfig(*context.db, bind_data.storage_config);
        }
    }

    static unique_ptr<FunctionData> BindEnableLogging(ClientContext &context, TableFunctionBindInput &input,
                                                      vector<LogicalType> &return_types, vector<string> &names) {
        if (input.inputs.size() > 1) {
            throw InvalidInputException("EnableLogging: expected 0 or 1 parameter");
        }

        auto result = make_uniq<EnableLoggingBindData>();

        bool storage_isset = false;
        bool storage_path_isset = false;

        for (const auto &param: input.named_parameters) {
            auto key = StringUtil::Lower(param.first);
            if (key == "level") {
                result->config.level = EnumUtil::FromString<LogLevel>(param.second.ToString());
            } else if (key == "storage") {
                storage_isset = true;
                result->config.storage = param.second.ToString();
            } else if (key == "storage_config") {
                if (param.second.type().id() != LogicalTypeId::STRUCT) {
                    throw InvalidInputException("EnableLogging: storage_config must be a struct");
                }
                auto &children = StructValue::GetChildren(param.second);
                for (idx_t i = 0; i < children.size(); i++) {
                    result->storage_config[StructType::GetChildName(param.second.type(), i)] = children[i];
                }
            } else if (key == "storage_path") {
                storage_path_isset = true;
                result->storage_config["path"] = param.second;
            } else if (key == "storage_normalize") {
                result->storage_config["normalize"] = param.second;
            } else if (key == "storage_buffer_size") {
                result->storage_config["buffer_size"] = param.second;
            } else {
                throw InvalidInputException("EnableLogging: unknown named parameter: %s", param.first.c_str());
            }
        }

        // This will implicitly set the log storage if the storage_path param is set and the storage is omitted
        if (!storage_isset && storage_path_isset) {
            result->config.storage = LogConfig::FILE_STORAGE_NAME;
        }

        // Process positional params
        if (!input.inputs.empty()) {
            if (input.inputs[0].type() == LogicalType::VARCHAR) {
                result->log_types_to_set.push_back(input.inputs[0].GetValue<string>());
            } else if (input.inputs[0].type() == LogicalType::LIST(LogicalType::VARCHAR)) {
                for (const auto &child: ListValue::GetChildren(input.inputs[0])) {
                    result->log_types_to_set.push_back(child.GetValue<string>());
                }
            } else {
                throw BinderException("Unexpected type positional parameter to enable_logging");
            }
        }

        return_types.emplace_back(LogicalType::BOOLEAN);
        names.emplace_back("Success");

        return result;
    }

    //! Reset the logmanager to the defaults
    static void DisableLogging(ClientContext &context, TableFunctionInput &data, DataChunk &output) {
        context.db->GetLogManager().SetEnableLogging(false);
    }

    //! Truncate the current log storage
    static void TruncateLogs(ClientContext &context, TableFunctionInput &data, DataChunk &output) {
        context.db->GetLogManager().TruncateLogStorage();
    }

    static unique_ptr<FunctionData> BindDisableLogging(ClientContext &context, TableFunctionBindInput &input,
                                                       vector<LogicalType> &return_types, vector<string> &names) {
        return_types.emplace_back(LogicalType::BOOLEAN);
        names.emplace_back("Success");

        return std::move(make_uniq<EnableLoggingBindData>());
    }

    static unique_ptr<FunctionData> BindTruncateLogs(ClientContext &context, TableFunctionBindInput &input,
                                                     vector<LogicalType> &return_types, vector<string> &names) {
        return_types.emplace_back(LogicalType::BOOLEAN);
        names.emplace_back("Success");

        return make_uniq<EnableLoggingBindData>();
    }

    void EnableLoggingFun::RegisterFunction(BuiltinFunctions &set) {
        auto enable_fun = TableFunction("enable_logging", {}, EnableLogging, BindEnableLogging, nullptr, nullptr);

        // Base config
        enable_fun.named_parameters.emplace("level", LogicalType::VARCHAR);
        enable_fun.named_parameters.emplace("storage", LogicalType::VARCHAR);
        enable_fun.named_parameters.emplace("storage_config", LogicalType::ANY);

        // Config that is forwarded to the storage_config struct as syntactic sugar
        enable_fun.named_parameters.emplace("storage_path", LogicalType::VARCHAR);
        enable_fun.named_parameters.emplace("storage_normalize", LogicalType::BOOLEAN);
        enable_fun.named_parameters.emplace("storage_buffer_size", LogicalType::UBIGINT);

        enable_fun.varargs = LogicalType::ANY;
        set.AddFunction(enable_fun);

        auto disable_fun = TableFunction("disable_logging", {}, DisableLogging, BindDisableLogging, nullptr, nullptr);
        set.AddFunction(disable_fun);

        auto truncate_fun = TableFunction("truncate_goose_logs", {}, TruncateLogs, BindTruncateLogs, nullptr, nullptr);
        set.AddFunction(truncate_fun);
    }
} // namespace goose
