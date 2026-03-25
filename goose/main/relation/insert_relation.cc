#include <goose/main/relation/insert_relation.h>
#include <goose/parser/statement/insert_statement.h>
#include <goose/parser/statement/select_statement.h>
#include <goose/parser/parsed_data/create_table_info.h>
#include <goose/planner/binder.h>
#include <goose/main/client_context.h>

namespace goose {

InsertRelation::InsertRelation(shared_ptr<Relation> child_p, string schema_name, string table_name)
    : Relation(child_p->context, RelationType::INSERT_RELATION), child(std::move(child_p)),
      schema_name(std::move(schema_name)), table_name(std::move(table_name)) {
	TryBindRelation(columns);
}

InsertRelation::InsertRelation(shared_ptr<Relation> child_p, string catalog_name, string schema_name, string table_name)
    : Relation(child_p->context, RelationType::INSERT_RELATION), child(std::move(child_p)),
      catalog_name(std::move(catalog_name)), schema_name(std::move(schema_name)), table_name(std::move(table_name)) {
	TryBindRelation(columns);
}

BoundStatement InsertRelation::Bind(Binder &binder) {
	InsertStatement stmt;
	auto select = make_uniq<SelectStatement>();
	select->node = child->GetQueryNode();

	stmt.catalog = catalog_name;
	stmt.schema = schema_name;
	stmt.table = table_name;
	stmt.select_statement = std::move(select);
	return binder.Bind(stmt.Cast<SQLStatement>());
}

unique_ptr<QueryNode> InsertRelation::GetQueryNode() {
	throw InternalException("Cannot create a query node from an insert relation");
}

string InsertRelation::GetQuery() {
	return string();
}

const vector<ColumnDefinition> &InsertRelation::Columns() {
	return columns;
}

string InsertRelation::ToString(idx_t depth) {
	string str = RenderWhitespace(depth) + "Insert\n";
	return str + child->ToString(depth + 1);
}

} // namespace goose
