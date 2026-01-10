#include <goose/main/relation/subquery_relation.h>
#include <goose/main/client_context.h>
#include <goose/parser/query_node.h>

namespace goose {

SubqueryRelation::SubqueryRelation(shared_ptr<Relation> child_p, const string &alias_p)
    : Relation(child_p->context, RelationType::SUBQUERY_RELATION, alias_p), child(std::move(child_p)) {
	D_ASSERT(child.get() != this);
	vector<ColumnDefinition> dummy_columns;
	Relation::TryBindRelation(dummy_columns);
}

unique_ptr<QueryNode> SubqueryRelation::GetQueryNode() {
	return child->GetQueryNode();
}

const vector<ColumnDefinition> &SubqueryRelation::Columns() {
	return child->Columns();
}

string SubqueryRelation::ToString(idx_t depth) {
	return child->ToString(depth);
}

} // namespace goose
