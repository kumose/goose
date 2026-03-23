#include <goose/parser/transformer.h>
#include <goose/parser/expression/operator_expression.h>

namespace goose {

unique_ptr<ParsedExpression> Transformer::TransformGroupingFunction(cantor::PGGroupingFunc &grouping) {
	auto op = make_uniq<OperatorExpression>(ExpressionType::GROUPING_FUNCTION);
	for (auto node = grouping.args->head; node; node = node->next) {
		auto n = PGPointerCast<cantor::PGNode>(node->data.ptr_value);
		op->children.push_back(TransformExpression(n));
	}
	SetQueryLocation(*op, grouping.location);
	return std::move(op);
}

} // namespace goose
