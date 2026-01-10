#include "include/icu-dateadd.h"

#include <goose/main/extension/extension_loader.h>
#include <goose/common/types/time.h>
#include <goose/common/types/timestamp.h>
#include <goose/parser/parsed_data/create_scalar_function_info.h>
#include <goose/planner/expression/bound_function_expression.h>
#include <goose/transaction/meta_transaction.h>
#include "include/icu-current.h"
#include "include/icu-casts.h"

namespace goose {

static timestamp_t GetTransactionTimestamp(ExpressionState &state) {
	return MetaTransaction::Get(state.GetContext()).start_timestamp;
}

static void CurrentTimeFunction(DataChunk &input, ExpressionState &state, Vector &result) {
	D_ASSERT(input.ColumnCount() == 0);
	auto instant = GetTransactionTimestamp(state);
	ICUDateFunc::BindData data(state.GetContext());

	dtime_tz_t result_time(dtime_t(0), 0);
	ICUToTimeTZ::ToTimeTZ(data.calendar.get(), instant, result_time);
	auto val = Value::TIMETZ(result_time);
	result.Reference(val);
}

static void CurrentDateFunction(DataChunk &input, ExpressionState &state, Vector &result) {
	D_ASSERT(input.ColumnCount() == 0);
	auto instant = GetTransactionTimestamp(state);

	auto val = Value::DATE(ICUMakeDate::ToDate(state.GetContext(), instant));
	result.Reference(val);
}

ScalarFunction GetCurrentTimeFun() {
	ScalarFunction current_time({}, LogicalType::TIME_TZ, CurrentTimeFunction);
	current_time.SetStability(FunctionStability::CONSISTENT_WITHIN_QUERY);
	return current_time;
}

ScalarFunction GetCurrentDateFun() {
	ScalarFunction current_date({}, LogicalType::DATE, CurrentDateFunction);
	current_date.SetStability(FunctionStability::CONSISTENT_WITHIN_QUERY);
	return current_date;
}

void RegisterICUCurrentFunctions(ExtensionLoader &loader) {
	//	temporal + interval
	ScalarFunctionSet current_time("get_current_time");
	current_time.AddFunction(GetCurrentTimeFun());
	loader.RegisterFunction(current_time);

	ScalarFunctionSet current_date("current_date");
	current_date.AddFunction(GetCurrentDateFun());
	loader.RegisterFunction(current_date);

	current_date.name = "today";
	loader.RegisterFunction(current_date);
}

} // namespace goose
