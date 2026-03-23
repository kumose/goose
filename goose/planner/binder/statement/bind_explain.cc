#include <goose/planner/binder.h>
#include <goose/parser/statement/explain_statement.h>
#include <goose/planner/operator/logical_explain.h>

namespace goose {

BoundStatement Binder::Bind(ExplainStatement &stmt) {
	BoundStatement result;

	// bind the underlying statement
	auto plan = Bind(*stmt.stmt);
	// get the unoptimized logical plan, and create the explain statement
	auto logical_plan_unopt = plan.plan->ToString(stmt.explain_format);
	auto explain = make_uniq<LogicalExplain>(std::move(plan.plan), stmt.explain_type, stmt.explain_format);
	explain->logical_plan_unopt = logical_plan_unopt;

	result.plan = std::move(explain);
	result.names = {"explain_key", "explain_value"};
	result.types = {LogicalType::VARCHAR, LogicalType::VARCHAR};

	auto &properties = GetStatementProperties();
	properties.return_type = StatementReturnType::QUERY_RESULT;
	return result;
}

} // namespace goose
