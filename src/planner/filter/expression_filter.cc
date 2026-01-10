#include <goose/planner/filter/expression_filter.h>
#include <goose/planner/expression_iterator.h>
#include <goose/planner/expression/bound_reference_expression.h>
#include <goose/common/types/data_chunk.h>
#include <goose/execution/expression_executor.h>

namespace goose {

ExpressionFilter::ExpressionFilter(unique_ptr<Expression> expr_p)
    : TableFilter(TableFilterType::EXPRESSION_FILTER), expr(std::move(expr_p)) {
}

bool ExpressionFilter::EvaluateWithConstant(ClientContext &context, const Value &val) {
	ExpressionExecutor executor(context, *expr);
	return EvaluateWithConstant(executor, val);
}

bool ExpressionFilter::EvaluateWithConstant(ExpressionExecutor &executor, const Value &val) const {
	DataChunk input;
	input.data.emplace_back(val);
	input.SetCardinality(1);

	SelectionVector sel(1);

	idx_t count = executor.SelectExpression(input, sel);
	return count > 0;
}

FilterPropagateResult ExpressionFilter::CheckStatistics(BaseStatistics &stats) const {
	if (stats.GetStatsType() == StatisticsType::GEOMETRY_STATS) {
		// Delegate to GeometryStats for geometry types
		return GeometryStats::CheckZonemap(stats, expr);
	}

	// we cannot prune based on arbitrary expressions currently
	return FilterPropagateResult::NO_PRUNING_POSSIBLE;
}

string ExpressionFilter::ToString(const string &column_name) const {
	auto name_expr = make_uniq<BoundReferenceExpression>(column_name, LogicalType::INVALID, 0ULL);
	return ToExpression(*name_expr)->ToString();
}

void ExpressionFilter::ReplaceExpressionRecursive(unique_ptr<Expression> &expr, const Expression &column,
                                                  ExpressionType replace_type) {
	if (expr->type == replace_type) {
		expr = column.Copy();
		return;
	}
	ExpressionIterator::EnumerateChildren(
	    *expr, [&](unique_ptr<Expression> &child) { ReplaceExpressionRecursive(child, column, replace_type); });
}

unique_ptr<Expression> ExpressionFilter::ToExpression(const Expression &column) const {
	auto expr_copy = expr->Copy();
	ReplaceExpressionRecursive(expr_copy, column);
	return expr_copy;
}

bool ExpressionFilter::Equals(const TableFilter &other_p) const {
	if (!TableFilter::Equals(other_p)) {
		return false;
	}
	auto &other = other_p.Cast<ExpressionFilter>();
	return other.expr->Equals(*expr);
}

unique_ptr<TableFilter> ExpressionFilter::Copy() const {
	return make_uniq<ExpressionFilter>(expr->Copy());
}

} // namespace goose
