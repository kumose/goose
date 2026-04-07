#include <goose/function/table/system_functions.h>

#include <goose/main/config.h>
#include <goose/main/client_context.h>
#include <goose/common/enum_util.h>
#include <goose/common/enums/optimizer_type.h>

namespace goose {
    struct GooseOptimizersData : public GlobalTableFunctionState {
        GooseOptimizersData() : offset(0) {
        }

        vector<string> optimizers;
        idx_t offset;
    };

    static unique_ptr<FunctionData> GooseOptimizersBind(ClientContext &context, TableFunctionBindInput &input,
                                                        vector<LogicalType> &return_types, vector<string> &names) {
        names.emplace_back("name");
        return_types.emplace_back(LogicalType::VARCHAR);

        return nullptr;
    }

    unique_ptr<GlobalTableFunctionState> GooseOptimizersInit(ClientContext &context, TableFunctionInitInput &input) {
        auto result = make_uniq<GooseOptimizersData>();
        result->optimizers = ListAllOptimizers();
        return std::move(result);
    }

    void GooseOptimizersFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &data = data_p.global_state->Cast<GooseOptimizersData>();
        if (data.offset >= data.optimizers.size()) {
            // finished returning values
            return;
        }
        // start returning values
        // either fill up the chunk or return all the remaining columns
        idx_t count = 0;
        while (data.offset < data.optimizers.size() && count < STANDARD_VECTOR_SIZE) {
            auto &entry = data.optimizers[data.offset++];

            // return values:
            // name, LogicalType::VARCHAR
            output.SetValue(0, count, Value(entry));
            count++;
        }
        output.SetCardinality(count);
    }

    void GooseOptimizersFun::RegisterFunction(BuiltinFunctions &set) {
        set.AddFunction(
            TableFunction("goose_optimizers", {}, GooseOptimizersFunction, GooseOptimizersBind, GooseOptimizersInit));
    }
} // namespace goose
