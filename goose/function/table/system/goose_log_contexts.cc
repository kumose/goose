#include <goose/function/table/system_functions.h>

#include <goose/catalog/catalog.h>
#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/common/exception.h>
#include <goose/main/client_context.h>
#include <goose/main/client_data.h>
#include <goose/logging/log_manager.h>
#include <goose/logging/log_storage.h>

namespace goose {

struct GooseLogContextData : public GlobalTableFunctionState {
	explicit GooseLogContextData(shared_ptr<LogStorage> log_storage_p) : log_storage(std::move(log_storage_p)) {
		scan_state = log_storage->CreateScanState(LoggingTargetTable::LOG_CONTEXTS);
		log_storage->InitializeScan(*scan_state);
	}
	GooseLogContextData() : log_storage(nullptr) {
	}

	//! The log storage we are scanning
	shared_ptr<LogStorage> log_storage;
	unique_ptr<LogStorageScanState> scan_state;
};

static unique_ptr<FunctionData> GooseLogContextBind(ClientContext &context, TableFunctionBindInput &input,
                                                     vector<LogicalType> &return_types, vector<string> &names) {
	names.emplace_back("context_id");
	return_types.emplace_back(LogicalType::UBIGINT);

	names.emplace_back("scope");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("connection_id");
	return_types.emplace_back(LogicalType::UBIGINT);

	names.emplace_back("transaction_id");
	return_types.emplace_back(LogicalType::UBIGINT);

	names.emplace_back("query_id");
	return_types.emplace_back(LogicalType::UBIGINT);

	names.emplace_back("thread_id");
	return_types.emplace_back(LogicalType::UBIGINT);

	return nullptr;
}

unique_ptr<GlobalTableFunctionState> GooseLogContextInit(ClientContext &context, TableFunctionInitInput &input) {
	if (LogManager::Get(context).CanScan(LoggingTargetTable::LOG_CONTEXTS)) {
		return make_uniq<GooseLogContextData>(LogManager::Get(context).GetLogStorage());
	}
	return make_uniq<GooseLogContextData>();
}

void GooseLogContextFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	auto &data = data_p.global_state->Cast<GooseLogContextData>();
	if (data.log_storage) {
		data.log_storage->Scan(*data.scan_state, output);
	}
}

static unique_ptr<TableRef> GooseLogContextsBindReplace(ClientContext &context, TableFunctionBindInput &input) {
	auto log_storage = LogManager::Get(context).GetLogStorage();

	// Attempt to let the storage BindReplace the scan function
	return log_storage->BindReplace(context, input, LoggingTargetTable::LOG_CONTEXTS);
}

void GooseLogContextFun::RegisterFunction(BuiltinFunctions &set) {
	auto fun =
	    TableFunction("goose_log_contexts", {}, GooseLogContextFunction, GooseLogContextBind, GooseLogContextInit);
	fun.bind_replace = GooseLogContextsBindReplace;
	set.AddFunction(fun);
}

} // namespace goose
