#pragma once
#include <goose/planner/expression/bound_aggregate_expression.h>
#include <goose/optimizer/matcher/expression_matcher.h>

namespace goose {

class AggregateFunctionExpressionMatcher : public ExpressionMatcher {
public:
	AggregateFunctionExpressionMatcher()
	    : ExpressionMatcher(ExpressionClass::BOUND_AGGREGATE), policy(SetMatcher::Policy::INVALID) {
	}
	//! The matchers for the child expressions
	vector<unique_ptr<ExpressionMatcher>> matchers;
	//! The set matcher matching policy to use
	SetMatcher::Policy policy;
	//! The function name to match
	unique_ptr<FunctionMatcher> function;

	bool Match(Expression &expr_p, vector<reference<Expression>> &bindings) override;
};

inline bool AggregateFunctionExpressionMatcher::Match(Expression &expr_p, vector<reference<Expression>> &bindings) {
	if (!ExpressionMatcher::Match(expr_p, bindings)) {
		return false;
	}
	auto &expr = expr_p.Cast<BoundAggregateExpression>();
	if (!FunctionMatcher::Match(function, expr.function.name)) {
		return false;
	}
	if (!SetMatcher::Match(matchers, expr.children, bindings, policy)) {
		return false;
	}
	return true;
}

} // namespace goose
