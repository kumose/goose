#include <goose/function/table/system_functions.h>

#include <goose/main/client_context.h>
#include <goose/main/database.h>
#include <goose/main/connection_manager.h>

namespace goose {
    struct GooseConnectionCountData : public GlobalTableFunctionState {
        GooseConnectionCountData() : count(0), finished(false) {
        }

        idx_t count;
        bool finished;
    };

    static unique_ptr<FunctionData> GooseConnectionCountBind(ClientContext &context, TableFunctionBindInput &input,
                                                             vector<LogicalType> &return_types, vector<string> &names) {
        names.emplace_back("count");
        return_types.emplace_back(LogicalType::UBIGINT);
        return nullptr;
    }

    unique_ptr<GlobalTableFunctionState>
    GooseConnectionCountInit(ClientContext &context, TableFunctionInitInput &input) {
        auto result = make_uniq<GooseConnectionCountData>();
        auto &conn_manager = context.db->GetConnectionManager();
        result->count = conn_manager.GetConnectionCount();
        return std::move(result);
    }

    void GooseConnectionCountFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
        auto &data = data_p.global_state->Cast<GooseConnectionCountData>();
        if (data.finished) {
            return;
        }
        output.SetValue(0, 0, Value::UBIGINT(data.count));
        output.SetCardinality(1);
        data.finished = true;
    }

    void GooseConnectionCountFun::RegisterFunction(BuiltinFunctions &set) {
        set.AddFunction(TableFunction("goose_connection_count", {}, GooseConnectionCountFunction,
                                      GooseConnectionCountBind, GooseConnectionCountInit));
    }
} // namespace goose
