#include <goose/parser/expression/constant_expression.h>
#include <goose/planner/expression/bound_constant_expression.h>
#include <goose/planner/expression_binder.h>

namespace goose {

BindResult ExpressionBinder::BindExpression(ConstantExpression &expr, idx_t depth) {
	return BindResult(make_uniq<BoundConstantExpression>(expr.value));
}

} // namespace goose
