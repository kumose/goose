#include <goose/planner/expression_binder/returning_binder.h>

#include <goose/planner/expression/bound_default_expression.h>

namespace goose {

ReturningBinder::ReturningBinder(Binder &binder, ClientContext &context) : ExpressionBinder(binder, context) {
}

BindResult ReturningBinder::BindExpression(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth, bool root_expression) {
	auto &expr = *expr_ptr;
	switch (expr.GetExpressionClass()) {
	case ExpressionClass::SUBQUERY:
		return BindResult(BinderException::Unsupported(expr, "SUBQUERY is not supported in returning statements"));
	case ExpressionClass::BOUND_SUBQUERY:
		return BindResult(
		    BinderException::Unsupported(expr, "BOUND SUBQUERY is not supported in returning statements"));
	case ExpressionClass::COLUMN_REF:
		return ExpressionBinder::BindExpression(expr_ptr, depth);
	default:
		return ExpressionBinder::BindExpression(expr_ptr, depth);
	}
}

} // namespace goose
