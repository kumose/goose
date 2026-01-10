#include <goose/parser/expression/parameter_expression.h>

#include <goose/common/exception.h>
#include <goose/common/types/hash.h>

namespace goose {

ParameterExpression::ParameterExpression()
    : ParsedExpression(ExpressionType::VALUE_PARAMETER, ExpressionClass::PARAMETER) {
}

string ParameterExpression::ToString() const {
	return "$" + identifier;
}

unique_ptr<ParsedExpression> ParameterExpression::Copy() const {
	auto copy = make_uniq<ParameterExpression>();
	copy->identifier = identifier;
	copy->CopyProperties(*this);
	return std::move(copy);
}

bool ParameterExpression::Equal(const ParameterExpression &a, const ParameterExpression &b) {
	return StringUtil::CIEquals(a.identifier, b.identifier);
}

hash_t ParameterExpression::Hash() const {
	hash_t result = ParsedExpression::Hash();
	return CombineHash(goose::Hash(identifier.c_str(), identifier.size()), result);
}

} // namespace goose
