#include <goose/parser/transformer.h>
#include <goose/parser/expression/function_expression.h>
#include <goose/parser/statement/call_statement.h>
#include <goose/parser/expression/constant_expression.h>

namespace goose {

unique_ptr<SQLStatement> Transformer::TransformCheckpoint(cantor::PGCheckPointStmt &stmt) {
	vector<unique_ptr<ParsedExpression>> children;
	// transform into "CALL checkpoint()" or "CALL force_checkpoint()"
	auto checkpoint_name = stmt.force ? "force_checkpoint" : "checkpoint";
	auto result = make_uniq<CallStatement>();
	auto function = make_uniq<FunctionExpression>(checkpoint_name, std::move(children));
	function->catalog = SYSTEM_CATALOG;
	function->schema = DEFAULT_SCHEMA;
	if (stmt.name) {
		function->children.push_back(make_uniq<ConstantExpression>(Value(stmt.name)));
	}
	result->function = std::move(function);
	return std::move(result);
}

} // namespace goose
