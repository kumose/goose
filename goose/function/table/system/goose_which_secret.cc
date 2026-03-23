#include <goose/function/table/system_functions.h>

#include <goose/common/file_system.h>
#include <goose/common/types-import.h>
#include <goose/common/string_util.h>
#include <goose/common/multi_file/multi_file_reader.h>
#include <goose/function/function_set.h>
#include <goose/main/client_context.h>
#include <goose/main/database.h>
#include <goose/main/extension_helper.h>
#include <goose/main/secret/secret_manager.h>

namespace goose {

struct GooseWhichSecretData : public GlobalTableFunctionState {
	GooseWhichSecretData() : finished(false) {
	}
	bool finished;
};

struct GooseWhichSecretBindData : public TableFunctionData {
	explicit GooseWhichSecretBindData(TableFunctionBindInput &tf_input) : inputs(tf_input.inputs) {};

	goose::vector<goose::Value> inputs;
};

static unique_ptr<FunctionData> GooseWhichSecretBind(ClientContext &context, TableFunctionBindInput &input,
                                                      vector<LogicalType> &return_types, vector<string> &names) {
	names.emplace_back("name");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("persistent");
	return_types.emplace_back(LogicalType::VARCHAR);

	names.emplace_back("storage");
	return_types.emplace_back(LogicalType::VARCHAR);

	return make_uniq<GooseWhichSecretBindData>(input);
}

unique_ptr<GlobalTableFunctionState> GooseWhichSecretInit(ClientContext &context, TableFunctionInitInput &input) {
	return make_uniq<GooseWhichSecretData>();
}

void GooseWhichSecretFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	auto &data = data_p.global_state->Cast<GooseWhichSecretData>();
	if (data.finished) {
		// finished returning values
		return;
	}
	auto &bind_data = data_p.bind_data->Cast<GooseWhichSecretBindData>();

	auto &secret_manager = SecretManager::Get(context);
	auto transaction = CatalogTransaction::GetSystemCatalogTransaction(context);

	auto &inputs = bind_data.inputs;
	auto path = inputs[0].ToString();
	auto type = inputs[1].ToString();
	auto secret_match = secret_manager.LookupSecret(transaction, path, type);
	if (secret_match.HasMatch()) {
		auto &secret_entry = *secret_match.secret_entry;
		output.SetCardinality(1);
		output.SetValue(0, 0, secret_entry.secret->GetName());
		output.SetValue(1, 0, EnumUtil::ToString(secret_entry.persist_type));
		output.SetValue(2, 0, secret_entry.storage_mode);
	}
	data.finished = true;
}

void GooseWhichSecretFun::RegisterFunction(BuiltinFunctions &set) {
	set.AddFunction(TableFunction("which_secret", {goose::LogicalType::VARCHAR, goose::LogicalType::VARCHAR},
	                              GooseWhichSecretFunction, GooseWhichSecretBind, GooseWhichSecretInit));
}

} // namespace goose
