#include <goose/main/relation/materialized_relation.h>
#include <goose/parser/query_node/select_node.h>
#include <goose/main/client_context.h>
#include <goose/planner/bound_statement.h>
#include <goose/planner/operator/logical_column_data_get.h>
#include <goose/parser/tableref/column_data_ref.h>
#include <goose/parser/expression/star_expression.h>
#include <goose/common/exception.h>

namespace goose {

MaterializedRelation::MaterializedRelation(const shared_ptr<ClientContext> &context,
                                           unique_ptr<ColumnDataCollection> &&collection_p, vector<string> names,
                                           string alias_p)
    : Relation(context, RelationType::MATERIALIZED_RELATION), alias(std::move(alias_p)),
      collection(std::move(collection_p)) {
	// create constant expressions for the values
	auto types = collection->Types();
	D_ASSERT(types.size() == names.size());

	QueryResult::DeduplicateColumns(names);
	for (idx_t i = 0; i < types.size(); i++) {
		auto &type = types[i];
		auto &name = names[i];
		auto column_definition = ColumnDefinition(name, type);
		columns.push_back(std::move(column_definition));
	}
}

unique_ptr<QueryNode> MaterializedRelation::GetQueryNode() {
	auto result = make_uniq<SelectNode>();
	result->select_list.push_back(make_uniq<StarExpression>());
	result->from_table = GetTableRef();
	return std::move(result);
}

unique_ptr<TableRef> MaterializedRelation::GetTableRef() {
	auto table_ref = make_uniq<ColumnDataRef>(collection);
	for (auto &col : columns) {
		table_ref->expected_names.push_back(col.Name());
	}
	table_ref->alias = GetAlias();
	return std::move(table_ref);
}

string MaterializedRelation::GetAlias() {
	return alias;
}

const vector<ColumnDefinition> &MaterializedRelation::Columns() {
	return columns;
}

string MaterializedRelation::ToString(idx_t depth) {
	return collection->ToString();
}

} // namespace goose
