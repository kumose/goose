#include <goose/extension/core_functions/scalar/map_functions.h>
#include <goose/planner/expression/bound_function_expression.h>
#include <goose/common/string_util.h>
#include <goose/parser/expression/bound_expression.h>
#include <goose/common/types/data_chunk.h>
#include <goose/function/scalar/nested_functions.h>

namespace goose {

static void MapFromEntriesFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	auto count = args.size();

	MapUtil::ReinterpretMap(result, args.data[0], count);
	MapVector::MapConversionVerify(result, count);
	result.Verify(count);

	if (args.AllConstant()) {
		result.SetVectorType(VectorType::CONSTANT_VECTOR);
	}
}

ScalarFunction MapFromEntriesFun::GetFunction() {
	auto key_type = LogicalType::TEMPLATE("K");
	auto val_type = LogicalType::TEMPLATE("V");
	auto map_type = LogicalType::MAP(key_type, val_type);
	auto row_type = LogicalType::STRUCT({{"", key_type}, {"", val_type}});

	ScalarFunction fun({LogicalType::LIST(row_type)}, map_type, MapFromEntriesFunction);
	fun.SetNullHandling(FunctionNullHandling::DEFAULT_NULL_HANDLING);

	fun.SetFallible();
	return fun;
}

} // namespace goose
