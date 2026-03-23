#include <goose/parser/expression/operator_expression.h>
#include <goose/parser/transformer.h>

namespace goose {

// COALESCE(a,b,c) returns the first argument that is NOT NULL, so
// rewrite into CASE(a IS NOT NULL, a, CASE(b IS NOT NULL, b, c))
unique_ptr<ParsedExpression> Transformer::TransformCoalesce(cantor::PGAExpr &root) {
	auto coalesce_args = PGPointerCast<cantor::PGList>(root.lexpr);
	D_ASSERT(coalesce_args->length > 0); // parser ensures this already

	auto coalesce_op = make_uniq<OperatorExpression>(ExpressionType::OPERATOR_COALESCE);
	for (auto cell = coalesce_args->head; cell; cell = cell->next) {
		// get the value of the COALESCE
		auto value_expr = TransformExpression(PGPointerCast<cantor::PGNode>(cell->data.ptr_value));
		coalesce_op->children.push_back(std::move(value_expr));
	}
	return std::move(coalesce_op);
}

} // namespace goose
