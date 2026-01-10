#include "delta_functions.hpp"

#include <goose/goose.h>
#include <goose/main/extension_util.h>
#include <goose/parser/parsed_data/create_scalar_function_info.h>

namespace goose {

vector<TableFunctionSet> DeltaFunctions::GetTableFunctions(DatabaseInstance &instance) {
	vector<TableFunctionSet> functions;

	functions.push_back(GetDeltaScanFunction(instance));

	return functions;
}

}; // namespace goose
