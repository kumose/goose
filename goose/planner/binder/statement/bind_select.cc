#include <goose/parser/statement/select_statement.h>
#include <goose/planner/binder.h>
#include <goose/planner/bound_query_node.h>

namespace goose {

BoundStatement Binder::Bind(SelectStatement &stmt) {
	auto &properties = GetStatementProperties();
	properties.output_type = QueryResultOutputType::ALLOW_STREAMING;
	properties.return_type = StatementReturnType::QUERY_RESULT;
	return Bind(*stmt.node);
}

} // namespace goose
