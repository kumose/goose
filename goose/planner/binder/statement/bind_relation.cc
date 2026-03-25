#include <goose/catalog/catalog.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/statement/insert_statement.h>
#include <goose/parser/query_node/select_node.h>
#include <goose/planner/binder.h>
#include <goose/parser/statement/relation_statement.h>

namespace goose {

BoundStatement Binder::Bind(RelationStatement &stmt) {
	return stmt.relation->Bind(*this);
}

} // namespace goose
