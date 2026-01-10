#include <goose/planner/subquery/rewrite_cte_scan.h>

#include <goose/planner/operator/list.h>

#include <goose/planner/expression/bound_case_expression.h>
#include <goose/planner/expression/bound_columnref_expression.h>
#include <goose/planner/expression/bound_constant_expression.h>
#include <goose/planner/expression/bound_operator_expression.h>
#include <goose/planner/expression/bound_subquery_expression.h>
#include <goose/planner/query_node/bound_select_node.h>
#include <goose/planner/expression_iterator.h>
#include <goose/planner/tableref/bound_joinref.h>
#include <goose/planner/operator/logical_dependent_join.h>

namespace goose {

RewriteCTEScan::RewriteCTEScan(idx_t table_index, const CorrelatedColumns &correlated_columns,
                               bool rewrite_dependent_joins)
    : table_index(table_index), correlated_columns(correlated_columns),
      rewrite_dependent_joins(rewrite_dependent_joins) {
}

void RewriteCTEScan::VisitOperator(LogicalOperator &op) {
	if (op.type == LogicalOperatorType::LOGICAL_CTE_REF) {
		auto &cteref = op.Cast<LogicalCTERef>();

		if (cteref.cte_index == table_index && cteref.correlated_columns == 0) {
			for (auto &c : this->correlated_columns) {
				cteref.chunk_types.push_back(c.type);
				cteref.bound_columns.push_back(c.name);
			}
			cteref.correlated_columns += correlated_columns.size();
		}
	} else if (op.type == LogicalOperatorType::LOGICAL_DEPENDENT_JOIN && rewrite_dependent_joins) {
		// There is another DependentJoin below the correlated recursive CTE.
		// We have to add the correlated columns of the recursive CTE to the
		// set of columns of this operator.
		auto &join = op.Cast<LogicalDependentJoin>();

		for (auto &c : correlated_columns) {
			bool contains_binding = false;
			for (auto &col : join.correlated_columns) {
				if (col.binding == c.binding) {
					contains_binding = true;
					break;
				}
			}
			// We only add new columns
			if (!contains_binding) {
				CorrelatedColumnInfo corr = c;
				// The correlated columns must be placed at the beginning of the
				// correlated_columns list. Otherwise, further column accesses
				// and rewrites will fail.
				join.correlated_columns.AddColumn(std::move(corr));
			}
		}
	}
	VisitOperatorChildren(op);
}

} // namespace goose
