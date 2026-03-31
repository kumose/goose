#include <goose/common/exception.h>
#include <goose/parser/expression/lambda_expression.h>
#include <goose/parser/transformer.h>

namespace goose {
    unique_ptr<ParsedExpression> Transformer::TransformLambda(cantor::PGLambdaFunction &node) {
        D_ASSERT(node.lhs);
        D_ASSERT(node.rhs);

        auto parameters = TransformStringList(node.lhs);
        auto rhs = TransformExpression(node.rhs);
        D_ASSERT(!parameters.empty());
        D_ASSERT(rhs);
        auto result = make_uniq<LambdaExpression>(std::move(parameters), std::move(rhs));
        SetQueryLocation(*result, node.location);
        return std::move(result);
    }

    unique_ptr<ParsedExpression> Transformer::TransformSingleArrow(cantor::PGSingleArrowFunction &node) {
        D_ASSERT(node.lhs);
        D_ASSERT(node.rhs);

        auto lhs = TransformExpression(node.lhs);
        auto rhs = TransformExpression(node.rhs);
        D_ASSERT(lhs);
        D_ASSERT(rhs);
        auto result = make_uniq<LambdaExpression>(std::move(lhs), std::move(rhs));
        SetQueryLocation(*result, node.location);
        return std::move(result);
    }
} // namespace goose
