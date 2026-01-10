#include <goose/common/exception.h>
#include <goose/parser/transformer.h>
#include <goose/parser/expression/positional_reference_expression.h>

namespace goose {

unique_ptr<ParsedExpression> Transformer::TransformPositionalReference(cantor::PGPositionalReference &node) {
	if (node.position <= 0) {
		throw ParserException("Positional reference node needs to be >= 1");
	}
	auto result = make_uniq<PositionalReferenceExpression>(NumericCast<idx_t>(node.position));
	SetQueryLocation(*result, node.location);
	return std::move(result);
}

} // namespace goose
