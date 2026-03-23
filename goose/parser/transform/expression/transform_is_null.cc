#include <goose/common/exception.h>
#include <goose/parser/expression/operator_expression.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<ParsedExpression> Transformer::TransformNullTest(cantor::PGNullTest &root) {
	auto arg = TransformExpression(PGPointerCast<cantor::PGNode>(root.arg));
	if (root.argisrow) {
		throw NotImplementedException("IS NULL argisrow");
	}
	ExpressionType expr_type = (root.nulltesttype == cantor::PG_IS_NULL)
	                               ? ExpressionType::OPERATOR_IS_NULL
	                               : ExpressionType::OPERATOR_IS_NOT_NULL;

	auto result = make_uniq<OperatorExpression>(expr_type, std::move(arg));
	SetQueryLocation(*result, root.location);
	return std::move(result);
}

} // namespace goose
