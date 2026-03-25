#include <goose/function/table/system_functions.h>
#include <goose/main/config.h>
#include <goose/main/client_context.h>
#include <goose/common/enum_util.h>
#include <goose/main/secret/secret_manager.h>
#include <goose/main/secret/secret.h>

namespace goose {
    struct GooseSecretTypesData : public GlobalTableFunctionState {
        GooseSecretTypesData() : offset(0) {
        }

        vector<SecretType> types;
        idx_t offset;
    };

    static unique_ptr<FunctionData> GooseSecretTypesBind(ClientContext &context, TableFunctionBindInput &input,
                                                         vector<LogicalType> &return_types, vector<string> &names) {
        names.emplace_back("type");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("default_provider");
        return_types.emplace_back(LogicalType::VARCHAR);

        names.emplace_back("extension");
        return_types.emplace_back(LogicalType::VARCHAR);

        return nullptr;
    }

    unique_ptr<GlobalTableFunctionState> GooseSecretTypesInit(ClientContext &context, TableFunctionInitInput &input) {
        auto result = make_uniq<GooseSecretTypesData>();

        auto &secret_manager = SecretManager::Get(context);
        result->types = secret_manager.AllSecretTypes();

        return result;
    }

    void GooseSecretTypesFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &data = data_p.global_state->Cast<GooseSecretTypesData>();
        if (data.offset >= data.types.size()) {
            // finished returning values
            return;
        }
        // start returning values
        // either fill up the chunk or return all the remaining columns
        idx_t count = 0;
        while (data.offset < data.types.size() && count < STANDARD_VECTOR_SIZE) {
            auto &entry = data.types[data.offset++];

            // return values:
            // type, LogicalType::VARCHAR
            output.SetValue(0, count, Value(entry.name));
            // default_provider, LogicalType::VARCHAR
            output.SetValue(1, count, Value(entry.default_provider));
            // extension, LogicalType::VARCHAR
            output.SetValue(2, count, Value(entry.extension));

            count++;
        }
        output.SetCardinality(count);
    }

    void GooseSecretTypesFun::RegisterFunction(BuiltinFunctions &set) {
        set.AddFunction(TableFunction("goose_secret_types", {}, GooseSecretTypesFunction, GooseSecretTypesBind,
                                      GooseSecretTypesInit));
    }
} // namespace goose
