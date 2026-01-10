#include <goose/function/table/system_functions.h>
#include <goose/main/database_manager.h>

namespace goose {

struct GooseApproxDatabaseCountData : public GlobalTableFunctionState {
	GooseApproxDatabaseCountData() : count(0), finished(false) {
	}
	idx_t count;
	bool finished;
};

static unique_ptr<FunctionData> GooseApproxDatabaseCountBind(ClientContext &context, TableFunctionBindInput &input,
                                                              vector<LogicalType> &return_types,
                                                              vector<string> &names) {
	names.emplace_back("approx_count");
	return_types.emplace_back(LogicalType::UBIGINT);
	return nullptr;
}

unique_ptr<GlobalTableFunctionState> GooseApproxDatabaseCountInit(ClientContext &context,
                                                                   TableFunctionInitInput &input) {
	auto result = make_uniq<GooseApproxDatabaseCountData>();
	result->count = DatabaseManager::Get(context).ApproxDatabaseCount();
	return std::move(result);
}

void GooseApproxDatabaseCountFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	auto &data = data_p.global_state->Cast<GooseApproxDatabaseCountData>();
	if (data.finished) {
		return;
	}
	output.SetValue(0, 0, Value::UBIGINT(data.count));
	output.SetCardinality(1);
	data.finished = true;
}

void GooseApproxDatabaseCountFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(TableFunction("goose_approx_database_count", {}, GooseApproxDatabaseCountFunction,
	                              GooseApproxDatabaseCountBind, GooseApproxDatabaseCountInit));
}

} // namespace goose
