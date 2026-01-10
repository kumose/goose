#include <goose/parser/query_node/statement_node.h>
#include <goose/parser/statement/insert_statement.h>
#include <goose/parser/statement/update_statement.h>
#include <goose/parser/statement/delete_statement.h>
#include <goose/parser/statement/merge_into_statement.h>
#include <goose/planner/binder.h>

namespace goose {

BoundStatement Binder::BindNode(StatementNode &statement) {
	// switch on type here to ensure we bind WITHOUT ctes to prevent infinite recursion
	switch (statement.stmt.type) {
	case StatementType::INSERT_STATEMENT:
		return Bind(statement.stmt.Cast<InsertStatement>());
	case StatementType::DELETE_STATEMENT:
		return Bind(statement.stmt.Cast<DeleteStatement>());
	case StatementType::UPDATE_STATEMENT:
		return Bind(statement.stmt.Cast<UpdateStatement>());
	case StatementType::MERGE_INTO_STATEMENT:
		return Bind(statement.stmt.Cast<MergeIntoStatement>());
	default:
		return Bind(statement.stmt);
	}
}

} // namespace goose
