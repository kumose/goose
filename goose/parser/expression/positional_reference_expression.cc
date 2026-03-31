#include <goose/parser/expression/positional_reference_expression.h>

#include <goose/common/exception.h>
#include <goose/common/types/hash.h>
#include <goose/common/types-import.h>

namespace goose {
    PositionalReferenceExpression::PositionalReferenceExpression()
        : ParsedExpression(ExpressionType::POSITIONAL_REFERENCE, ExpressionClass::POSITIONAL_REFERENCE) {
    }

    PositionalReferenceExpression::PositionalReferenceExpression(idx_t index)
        : ParsedExpression(ExpressionType::POSITIONAL_REFERENCE, ExpressionClass::POSITIONAL_REFERENCE), index(index) {
    }

    string PositionalReferenceExpression::ToString() const {
        return "#" + to_string(index);
    }

    bool PositionalReferenceExpression::Equal(const PositionalReferenceExpression &a,
                                              const PositionalReferenceExpression &b) {
        return a.index == b.index;
    }

    unique_ptr<ParsedExpression> PositionalReferenceExpression::Copy() const {
        auto copy = make_uniq<PositionalReferenceExpression>(index);
        copy->CopyProperties(*this);
        return std::move(copy);
    }

    hash_t PositionalReferenceExpression::Hash() const {
        hash_t result = ParsedExpression::Hash();
        return CombineHash(goose::Hash(index), result);
    }
} // namespace goose
