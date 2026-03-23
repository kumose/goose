#include <goose/parser/transformer.h>
#include <goose/parser/expression/comparison_expression.h>
#include <goose/parser/expression/operator_expression.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/expression/cast_expression.h>

namespace goose {

static unique_ptr<ParsedExpression> TransformBooleanTestInternal(unique_ptr<ParsedExpression> argument,
                                                                 ExpressionType comparison_type, bool comparison_value,
                                                                 int query_location) {
	auto bool_value = make_uniq<ConstantExpression>(Value::BOOLEAN(comparison_value));
	Transformer::SetQueryLocation(*bool_value, query_location);
	// we cast the argument to bool to remove ambiguity wrt function binding on the comparison
	auto cast_argument = make_uniq<CastExpression>(LogicalType::BOOLEAN, std::move(argument));

	auto result = make_uniq<ComparisonExpression>(comparison_type, std::move(cast_argument), std::move(bool_value));
	Transformer::SetQueryLocation(*result, query_location);
	return std::move(result);
}

static unique_ptr<ParsedExpression> TransformBooleanTestIsNull(unique_ptr<ParsedExpression> argument,
                                                               ExpressionType operator_type, int query_location) {
	auto result = make_uniq<OperatorExpression>(operator_type, std::move(argument));
	Transformer::SetQueryLocation(*result, query_location);
	return std::move(result);
}

unique_ptr<ParsedExpression> Transformer::TransformBooleanTest(cantor::PGBooleanTest &node) {
	auto argument = TransformExpression(PGPointerCast<cantor::PGNode>(node.arg));

	switch (node.booltesttype) {
	case cantor::PGBoolTestType::PG_IS_TRUE:
		return TransformBooleanTestInternal(std::move(argument), ExpressionType::COMPARE_NOT_DISTINCT_FROM, true,
		                                    node.location);
	case cantor::PGBoolTestType::IS_NOT_TRUE:
		return TransformBooleanTestInternal(std::move(argument), ExpressionType::COMPARE_DISTINCT_FROM, true,
		                                    node.location);
	case cantor::PGBoolTestType::IS_FALSE:
		return TransformBooleanTestInternal(std::move(argument), ExpressionType::COMPARE_NOT_DISTINCT_FROM, false,
		                                    node.location);
	case cantor::PGBoolTestType::IS_NOT_FALSE:
		return TransformBooleanTestInternal(std::move(argument), ExpressionType::COMPARE_DISTINCT_FROM, false,
		                                    node.location);
	case cantor::PGBoolTestType::IS_UNKNOWN: // IS NULL
		return TransformBooleanTestIsNull(std::move(argument), ExpressionType::OPERATOR_IS_NULL, node.location);
	case cantor::PGBoolTestType::IS_NOT_UNKNOWN: // IS NOT NULL
		return TransformBooleanTestIsNull(std::move(argument), ExpressionType::OPERATOR_IS_NOT_NULL, node.location);
	default:
		throw NotImplementedException("Unknown boolean test type %d", node.booltesttype);
	}
}

} // namespace goose
