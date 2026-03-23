#include <goose/parser/expression/conjunction_expression.h>
#include <goose/parser/expression/operator_expression.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<ParsedExpression> Transformer::TransformBoolExpr(cantor::PGBoolExpr &root) {
	unique_ptr<ParsedExpression> result;
	for (auto node = root.args->head; node != nullptr; node = node->next) {
		auto next = TransformExpression(PGPointerCast<cantor::PGNode>(node->data.ptr_value));

		switch (root.boolop) {
		case cantor::PG_AND_EXPR: {
			if (!result) {
				result = std::move(next);
			} else {
				result = make_uniq<ConjunctionExpression>(ExpressionType::CONJUNCTION_AND, std::move(result),
				                                          std::move(next));
			}
			break;
		}
		case cantor::PG_OR_EXPR: {
			if (!result) {
				result = std::move(next);
			} else {
				result = make_uniq<ConjunctionExpression>(ExpressionType::CONJUNCTION_OR, std::move(result),
				                                          std::move(next));
			}
			break;
		}
		case cantor::PG_NOT_EXPR: {
			if (next->GetExpressionType() == ExpressionType::COMPARE_IN) {
				// convert COMPARE_IN to COMPARE_NOT_IN
				next->SetExpressionTypeUnsafe(ExpressionType::COMPARE_NOT_IN);
				result = std::move(next);
			} else if ((next->GetExpressionType() >= ExpressionType::COMPARE_EQUAL &&
			            next->GetExpressionType() <= ExpressionType::COMPARE_GREATERTHANOREQUALTO) ||
			           next->GetExpressionType() == ExpressionType::COMPARE_DISTINCT_FROM ||
			           next->GetExpressionType() == ExpressionType::COMPARE_NOT_DISTINCT_FROM) {
				// NOT on a comparison: we can negate the comparison
				// e.g. NOT(x > y) is equivalent to x <= y
				// NOT(x IS DISTINCT FROM y) is equivalent to x IS NOT DISTINCT FROM y
				next->SetExpressionTypeUnsafe(NegateComparisonExpression(next->GetExpressionType()));
				result = std::move(next);
			} else {
				result = make_uniq<OperatorExpression>(ExpressionType::OPERATOR_NOT, std::move(next));
			}
			break;
		}
		}
	}
	SetQueryLocation(*result, root.location);
	return result;
}

} // namespace goose
