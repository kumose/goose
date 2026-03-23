#include <goose/planner/expression/bound_comparison_expression.h>
#include <goose/parser/expression/comparison_expression.h>

namespace goose {

BoundComparisonExpression::BoundComparisonExpression(ExpressionType type, unique_ptr<Expression> left,
                                                     unique_ptr<Expression> right)
    : Expression(type, ExpressionClass::BOUND_COMPARISON, LogicalType::BOOLEAN), left(std::move(left)),
      right(std::move(right)) {
}

string BoundComparisonExpression::ToString() const {
	return ComparisonExpression::ToString<BoundComparisonExpression, Expression>(*this);
}

bool BoundComparisonExpression::Equals(const BaseExpression &other_p) const {
	if (!Expression::Equals(other_p)) {
		return false;
	}
	auto &other = other_p.Cast<BoundComparisonExpression>();
	if (!Expression::Equals(*left, *other.left)) {
		return false;
	}
	if (!Expression::Equals(*right, *other.right)) {
		return false;
	}
	return true;
}

unique_ptr<Expression> BoundComparisonExpression::Copy() const {
	auto copy = make_uniq<BoundComparisonExpression>(type, left->Copy(), right->Copy());
	copy->CopyProperties(*this);
	return std::move(copy);
}

} // namespace goose
