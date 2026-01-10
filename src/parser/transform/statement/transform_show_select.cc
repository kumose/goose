#include <goose/parser/sql_statement.h>
#include <goose/parser/transformer.h>
#include <goose/parser/expression/star_expression.h>
#include <goose/parser/statement/select_statement.h>
#include <goose/parser/query_node/select_node.h>
#include <goose/parser/tableref/showref.h>

namespace goose {

unique_ptr<QueryNode> Transformer::TransformShowSelect(cantor::PGVariableShowSelectStmt &stmt) {
	// we capture the select statement of SHOW
	auto select_node = make_uniq<SelectNode>();
	select_node->select_list.push_back(make_uniq<StarExpression>());

	auto show_ref = make_uniq<ShowRef>();
	show_ref->show_type = stmt.is_summary ? ShowType::SUMMARY : ShowType::DESCRIBE;
	show_ref->query = TransformSelectNode(*stmt.stmt);
	select_node->from_table = std::move(show_ref);
	return std::move(select_node);
}

unique_ptr<SelectStatement> Transformer::TransformShowSelectStmt(cantor::PGVariableShowSelectStmt &stmt) {
	auto result = make_uniq<SelectStatement>();
	result->node = TransformShowSelect(stmt);
	return result;
}

} // namespace goose
