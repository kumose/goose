#include <goose/extension/core_functions/scalar/map_functions.h>
#include <goose/planner/expression/bound_function_expression.h>
#include <goose/common/string_util.h>
#include <goose/parser/expression/bound_expression.h>
#include <goose/common/types/data_chunk.h>
#include <goose/common/types-import.h>
#include <goose/function/scalar/nested_functions.h>

namespace goose {

// Reverse of map_from_entries
static void MapEntriesFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto count = args.size();

	auto &map = args.data[0];
	if (map.GetType().id() == LogicalTypeId::SQLNULL) {
		// Input is a constant NULL
		result.SetVectorType(VectorType::CONSTANT_VECTOR);
		ConstantVector::SetNull(result, true);
		return;
	}

	MapUtil::ReinterpretMap(result, map, count);

	if (args.AllConstant()) {
		result.SetVectorType(VectorType::CONSTANT_VECTOR);
	}
	result.Verify(count);
}

ScalarFunction MapEntriesFun::GetFunction() {
	auto key_type = LogicalType::TEMPLATE("K");
	auto val_type = LogicalType::TEMPLATE("V");
	auto map_type = LogicalType::MAP(key_type, val_type);
	auto row_type = LogicalType::STRUCT({{"key", key_type}, {"value", val_type}});

	ScalarFunction fun({map_type}, LogicalType::LIST(row_type), MapEntriesFunction);
	fun.SetNullHandling(FunctionNullHandling::SPECIAL_HANDLING);
	return fun;
}

} // namespace goose
