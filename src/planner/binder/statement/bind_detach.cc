#include <goose/parser/statement/detach_statement.h>
#include <goose/planner/binder.h>
#include <goose/planner/operator/logical_simple.h>
#include <goose/main/config.h>

namespace goose {

BoundStatement Binder::Bind(DetachStatement &stmt) {
	BoundStatement result;

	result.plan = make_uniq<LogicalSimple>(LogicalOperatorType::LOGICAL_DETACH, std::move(stmt.info));
	result.names = {"Success"};
	result.types = {LogicalType::BOOLEAN};

	auto &properties = GetStatementProperties();
	properties.output_type = QueryResultOutputType::FORCE_MATERIALIZED;
	properties.return_type = StatementReturnType::NOTHING;
	return result;
}

} // namespace goose
