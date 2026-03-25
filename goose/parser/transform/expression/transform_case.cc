#include <goose/parser/expression/case_expression.h>
#include <goose/parser/expression/comparison_expression.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/transformer.h>

namespace goose {
    unique_ptr<ParsedExpression> Transformer::TransformCase(cantor::PGCaseExpr &root) {
        auto case_node = make_uniq<CaseExpression>();
        auto root_arg = TransformExpression(PGPointerCast<cantor::PGNode>(root.arg));
        for (auto cell = root.args->head; cell != nullptr; cell = cell->next) {
            CaseCheck case_check;

            auto w = PGPointerCast<cantor::PGCaseWhen>(cell->data.ptr_value);
            auto test_raw = TransformExpression(PGPointerCast<cantor::PGNode>(w->expr));
            unique_ptr<ParsedExpression> test;
            if (root_arg) {
                case_check.when_expr =
                        make_uniq<ComparisonExpression>(ExpressionType::COMPARE_EQUAL, root_arg->Copy(),
                                                        std::move(test_raw));
            } else {
                case_check.when_expr = std::move(test_raw);
            }
            case_check.then_expr = TransformExpression(PGPointerCast<cantor::PGNode>(w->result));
            case_node->case_checks.push_back(std::move(case_check));
        }

        if (root.defresult) {
            case_node->else_expr = TransformExpression(PGPointerCast<cantor::PGNode>(root.defresult));
        } else {
            case_node->else_expr = make_uniq<ConstantExpression>(Value(LogicalType::SQLNULL));
        }
        SetQueryLocation(*case_node, root.location);
        return case_node;
    }
} // namespace goose
