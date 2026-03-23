#include <goose/function/table/range.h>
#include <goose/function/table_function.h>
#include <goose/function/function_set.h>
#include <goose/common/file_system.h>
#include <goose/main/config.h>
#include <goose/common/multi_file/multi_file_reader.h>

namespace goose {

struct GlobFunctionBindData : public TableFunctionData {
	shared_ptr<MultiFileList> file_list;
};

static unique_ptr<FunctionData> GlobFunctionBind(ClientContext &context, TableFunctionBindInput &input,
                                                 vector<LogicalType> &return_types, vector<string> &names) {
	auto result = make_uniq<GlobFunctionBindData>();
	auto multi_file_reader = MultiFileReader::Create(input.table_function);
	result->file_list = multi_file_reader->CreateFileList(context, input.inputs[0], FileGlobOptions::ALLOW_EMPTY);
	return_types.emplace_back(LogicalType::VARCHAR);
	names.emplace_back("file");
	return result;
}

struct GlobFunctionState : public GlobalTableFunctionState {
	GlobFunctionState() {
	}

	MultiFileListScanData file_list_scan;
};

static unique_ptr<GlobalTableFunctionState> GlobFunctionInit(ClientContext &context, TableFunctionInitInput &input) {
	auto &bind_data = input.bind_data->Cast<GlobFunctionBindData>();
	auto res = make_uniq<GlobFunctionState>();

	bind_data.file_list->InitializeScan(res->file_list_scan);

	return res;
}

static void GlobFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	auto &bind_data = data_p.bind_data->Cast<GlobFunctionBindData>();
	auto &state = data_p.global_state->Cast<GlobFunctionState>();

	idx_t count = 0;
	while (count < STANDARD_VECTOR_SIZE) {
		OpenFileInfo file;
		if (!bind_data.file_list->Scan(state.file_list_scan, file)) {
			break;
		}
		output.data[0].SetValue(count++, file.path);
	}
	output.SetCardinality(count);
}

void GlobTableFunction::RegisterFunction(BuiltinFunctions &set) {
	TableFunction glob_function("glob", {LogicalType::VARCHAR}, GlobFunction, GlobFunctionBind, GlobFunctionInit);
	set.AddFunction(MultiFileReader::CreateFunctionSet(glob_function));
}

} // namespace goose
