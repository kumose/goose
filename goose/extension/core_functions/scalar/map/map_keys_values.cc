#include <goose/extension/core_functions/scalar/map_functions.h>
#include <goose/planner/expression/bound_function_expression.h>
#include <goose/common/string_util.h>
#include <goose/parser/expression/bound_expression.h>
#include <goose/common/types/data_chunk.h>
#include <goose/common/types-import.h>
#include <goose/function/scalar/nested_functions.h>

namespace goose {

static void MapKeyValueFunction(DataChunk &args, ExpressionState &state, Vector &result,
                                Vector &(*get_child_vector)(Vector &)) {
	auto &map = args.data[0];

	D_ASSERT(result.GetType().id() == LogicalTypeId::LIST);
	if (map.GetType().id() == LogicalTypeId::SQLNULL) {
		result.SetVectorType(VectorType::CONSTANT_VECTOR);
		ConstantVector::SetNull(result, true);
		return;
	}

	auto count = args.size();
	D_ASSERT(map.GetType().id() == LogicalTypeId::MAP);
	auto child = get_child_vector(map);

	auto &entries = ListVector::GetEntry(result);
	entries.Reference(child);

	UnifiedVectorFormat map_data;
	map.ToUnifiedFormat(count, map_data);

	D_ASSERT(result.GetVectorType() == VectorType::FLAT_VECTOR);
	FlatVector::SetData(result, map_data.data);
	FlatVector::SetValidity(result, map_data.validity);
	auto list_size = ListVector::GetListSize(map);
	ListVector::SetListSize(result, list_size);
	if (map.GetVectorType() == VectorType::DICTIONARY_VECTOR) {
		result.Slice(*map_data.sel, count);
	}
	if (args.AllConstant()) {
		result.SetVectorType(VectorType::CONSTANT_VECTOR);
	}
	result.Verify(count);
}

static void MapKeysFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	MapKeyValueFunction(args, state, result, MapVector::GetKeys);
}

static void MapValuesFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	MapKeyValueFunction(args, state, result, MapVector::GetValues);
}

ScalarFunction MapKeysFun::GetFunction() {
	//! the arguments and return types are actually set in the binder function
	auto key_type = LogicalType::TEMPLATE("K");
	auto val_type = LogicalType::TEMPLATE("V");

	ScalarFunction function({LogicalType::MAP(key_type, val_type)}, LogicalType::LIST(key_type), MapKeysFunction);
	function.SetNullHandling(FunctionNullHandling::SPECIAL_HANDLING);

	function.SetFallible();
	return function;
}

ScalarFunction MapValuesFun::GetFunction() {
	auto key_type = LogicalType::TEMPLATE("K");
	auto val_type = LogicalType::TEMPLATE("V");

	ScalarFunction function({LogicalType::MAP(key_type, val_type)}, LogicalType::LIST(val_type), MapValuesFunction);
	function.SetNullHandling(FunctionNullHandling::SPECIAL_HANDLING);

	function.SetFallible();
	return function;
}

} // namespace goose
