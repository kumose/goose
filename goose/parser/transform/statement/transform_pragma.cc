#include <goose/common/case_insensitive_map.h>
#include <goose/common/enum_util.h>
#include <goose/execution/expression_executor.h>
#include <goose/parser/expression/columnref_expression.h>
#include <goose/parser/expression/comparison_expression.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/statement/pragma_statement.h>
#include <goose/parser/statement/set_statement.h>
#include <goose/parser/transformer.h>

namespace goose {
    unique_ptr<SQLStatement> Transformer::TransformPragma(cantor::PGPragmaStmt &stmt) {
        auto result = make_uniq<PragmaStatement>();
        auto &info = *result->info;

        info.name = stmt.name;
        // parse the arguments, if any
        if (stmt.args) {
            for (auto cell = stmt.args->head; cell != nullptr; cell = cell->next) {
                auto node = PGPointerCast<cantor::PGNode>(cell->data.ptr_value);
                auto expr = TransformExpression(node);

                if (expr->GetExpressionType() == ExpressionType::COMPARE_EQUAL) {
                    auto &comp = expr->Cast<ComparisonExpression>();
                    if (comp.left->GetExpressionType() != ExpressionType::COLUMN_REF) {
                        throw ParserException("Named parameter requires a column reference on the LHS");
                    }
                    auto &columnref = comp.left->Cast<ColumnRefExpression>();
                    info.named_parameters.insert(make_pair(columnref.GetName(), std::move(comp.right)));
                } else if (expr->GetExpressionType() == ExpressionType::COLUMN_REF) {
                    auto &colref = expr->Cast<ColumnRefExpression>();
                    if (!colref.IsQualified()) {
                        info.parameters.emplace_back(make_uniq<ConstantExpression>(Value(colref.GetColumnName())));
                    } else {
                        info.parameters.emplace_back(make_uniq<ConstantExpression>(Value(expr->ToString())));
                    }
                } else {
                    info.parameters.emplace_back(std::move(expr));
                }
            }
        }
        // now parse the pragma type
        switch (stmt.kind) {
            case cantor::PG_PRAGMA_TYPE_NOTHING: {
                if (!info.parameters.empty() || !info.named_parameters.empty()) {
                    throw InternalException(
                        "PRAGMA statement that is not a call or assignment cannot contain parameters");
                }
                break;
            case cantor::PG_PRAGMA_TYPE_ASSIGNMENT:
                if (info.parameters.size() != 1) {
                    throw ParserException("PRAGMA statement with assignment should contain exactly one parameter");
                }
                if (!info.named_parameters.empty()) {
                    throw ParserException("PRAGMA statement with assignment cannot have named parameters");
                }
                // SQLite does not distinguish between:
                // "PRAGMA table_info='integers'"
                // "PRAGMA table_info('integers')"
                // for compatibility, any pragmas that match the SQLite ones are parsed as calls
                case_insensitive_set_t sqlite_compat_pragmas{"table_info"};
                if (sqlite_compat_pragmas.find(info.name) != sqlite_compat_pragmas.end()) {
                    break;
                }
                auto set_statement =
                        make_uniq<SetVariableStatement>(info.name, std::move(info.parameters[0]), SetScope::AUTOMATIC);
                return set_statement;
            }
            case cantor::PG_PRAGMA_TYPE_CALL:
                break;
            default:
                throw InternalException("Unknown pragma type");
        }

        return result;
    }
} // namespace goose
