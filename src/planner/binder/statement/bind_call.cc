#include <goose/parser/statement/call_statement.h>
#include <goose/parser/tableref/table_function_ref.h>
#include <goose/planner/binder.h>
#include <goose/parser/query_node/select_node.h>
#include <goose/parser/expression/star_expression.h>

namespace goose {

BoundStatement Binder::Bind(CallStatement &stmt) {
	SelectStatement select_statement;
	auto select_node = make_uniq<SelectNode>();
	auto table_function = make_uniq<TableFunctionRef>();
	table_function->function = std::move(stmt.function);
	select_node->select_list.push_back(make_uniq<StarExpression>());
	select_node->from_table = std::move(table_function);
	select_statement.node = std::move(select_node);

	auto result = Bind(select_statement);
	auto &properties = GetStatementProperties();
	properties.output_type = QueryResultOutputType::FORCE_MATERIALIZED;
	return result;
}

} // namespace goose
