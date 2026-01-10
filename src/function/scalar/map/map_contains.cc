#include <goose/function/scalar/list/contains_or_position.h>
#include <goose/planner/expression/bound_cast_expression.h>
#include <goose/function/scalar/map_functions.h>

namespace goose {

static void MapContainsFunction(DataChunk &input, ExpressionState &state, Vector &result) {
	const auto count = input.size();

	auto &map_vec = input.data[0];
	auto &key_vec = MapVector::GetKeys(map_vec);
	auto &arg_vec = input.data[1];

	ListSearchOp<bool>(map_vec, key_vec, arg_vec, result, count);

	if (count == 1) {
		result.SetVectorType(VectorType::CONSTANT_VECTOR);
	}
}

ScalarFunction MapContainsFun::GetFunction() {
	auto key_type = LogicalType::TEMPLATE("K");
	auto val_type = LogicalType::TEMPLATE("V");

	ScalarFunction fun("map_contains", {LogicalType::MAP(key_type, val_type), key_type}, LogicalType::BOOLEAN,
	                   MapContainsFunction);
	return fun;
}

} // namespace goose
