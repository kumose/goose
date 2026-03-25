#include <goose/main/relation/explain_relation.h>
#include <goose/parser/statement/explain_statement.h>
#include <goose/parser/statement/select_statement.h>
#include <goose/parser/parsed_data/create_view_info.h>
#include <goose/planner/binder.h>
#include <goose/main/client_context.h>

namespace goose {

ExplainRelation::ExplainRelation(shared_ptr<Relation> child_p, ExplainType type, ExplainFormat format)
    : Relation(child_p->context, RelationType::EXPLAIN_RELATION), child(std::move(child_p)), type(type),
      format(format) {
	TryBindRelation(columns);
}

BoundStatement ExplainRelation::Bind(Binder &binder) {
	auto select = make_uniq<SelectStatement>();
	select->node = child->GetQueryNode();
	ExplainStatement explain(std::move(select), type, format);
	return binder.Bind(explain.Cast<SQLStatement>());
}

unique_ptr<QueryNode> ExplainRelation::GetQueryNode() {
	throw InternalException("Cannot create a query node from an explain relation");
}

string ExplainRelation::GetQuery() {
	return string();
}

const vector<ColumnDefinition> &ExplainRelation::Columns() {
	return columns;
}

string ExplainRelation::ToString(idx_t depth) {
	string str = RenderWhitespace(depth) + "Explain\n";
	return str + child->ToString(depth + 1);
}

} // namespace goose
