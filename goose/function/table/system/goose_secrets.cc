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

#include <goose/common/file_system.h>
#include <goose/common/types-import.h>
#include <goose/common/string_util.h>
#include <goose/function/function_set.h>
#include <goose/main/client_context.h>
#include <goose/main/database.h>
#include <goose/main/extension_helper.h>
#include <goose/main/secret/secret_manager.h>

namespace goose {
    struct GooseSecretsData : public GlobalTableFunctionState {
        GooseSecretsData() : offset(0) {
        }

        idx_t offset;
        goose::vector<goose::SecretEntry> secrets;
    };

    struct GooseSecretsBindData : public FunctionData {
    public:
        unique_ptr<FunctionData> Copy() const override {
            return make_uniq<GooseSecretsBindData>();
        };

        bool Equals(const FunctionData &other_p) const override {
            auto &other = other_p.Cast<GooseSecretsBindData>();
            return redact == other.redact;
        }

        SecretDisplayType redact = SecretDisplayType::REDACTED;
    };

    static unique_ptr<FunctionData> GooseSecretsBind(ClientContext &context, TableFunctionBindInput &input,
                                                     vector<LogicalType> &return_types, vector<string> &names) {
        auto result = make_uniq<GooseSecretsBindData>();

        auto entry = input.named_parameters.find("redact");
        if (entry != input.named_parameters.end()) {
            if (entry->second.IsNull()) {
                throw InvalidInputException("Cannot use NULL as argument for redact");
            }
            if (BooleanValue::Get(entry->second)) {
                result->redact = SecretDisplayType::REDACTED;
            } else {
                result->redact = SecretDisplayType::UNREDACTED;
            }
        }

        if (!DBConfig::GetConfig(context).options.allow_unredacted_secrets &&
            result->redact == SecretDisplayType::UNREDACTED) {
            throw InvalidInputException("Displaying unredacted secrets is disabled");
        }

        names.emplace_back("name");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("type");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("provider");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("persistent");
        return_types.emplace_back(LogicalType::BOOLEAN);

        names.emplace_back("storage");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("scope");
        return_types.emplace_back(LogicalType::LIST(LogicalType::VARCHAR));

        names.emplace_back("secret_string");
        return_types.emplace_back(LogicalType::VARCHAR);

        return result;
    }

    unique_ptr<GlobalTableFunctionState> GooseSecretsInit(ClientContext &context, TableFunctionInitInput &input) {
        auto result = make_uniq<GooseSecretsData>();
        return result;
    }

    void GooseSecretsFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &data = data_p.global_state->Cast<GooseSecretsData>();
        auto &bind_data = data_p.bind_data->Cast<GooseSecretsBindData>();

        auto &secret_manager = SecretManager::Get(context);

        auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);

        if (data.secrets.empty()) {
            data.secrets = secret_manager.AllSecrets(transaction);
        }
        auto &secrets = data.secrets;
        if (data.offset >= secrets.size()) {
            // finished returning values
            return;
        }
        // start returning values
        // either fill up the chunk or return all the remaining columns
        idx_t count = 0;
        while (data.offset < secrets.size() && count < STANDARD_VECTOR_SIZE) {
            auto &secret_entry = secrets[data.offset];

            vector<Value> scope_value;
            for (const auto &scope_entry: secret_entry.secret->GetScope()) {
                scope_value.push_back(scope_entry);
            }

            const auto &secret = *secret_entry.secret;

            idx_t i = 0;
            // name
            output.SetValue(i++, count, secret.GetName());
            // type
            output.SetValue(i++, count, Value(secret.GetType()));
            // provider
            output.SetValue(i++, count, Value(secret.GetProvider()));
            // persistent
            output.SetValue(i++, count, Value(secret_entry.persist_type == SecretPersistType::PERSISTENT));
            // storage
            output.SetValue(i++, count, Value(secret_entry.storage_mode));
            // scope
            output.SetValue(i++, count, Value::LIST(LogicalType::VARCHAR, scope_value));
            // secret_string
            output.SetValue(i++, count, secret.ToString(bind_data.redact));

            data.offset++;
            count++;
        }
        output.SetCardinality(count);
    }

    void GooseSecretsFun::RegisterFunction(BuiltinFunctions &set) {
        TableFunctionSet functions("goose_secrets");
        auto fun = TableFunction({}, GooseSecretsFunction, GooseSecretsBind, GooseSecretsInit);
        fun.named_parameters["redact"] = LogicalType::BOOLEAN;
        functions.AddFunction(fun);
        set.AddFunction(functions);
    }
} // namespace goose
