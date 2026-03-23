#include <goose/parser/statement/copy_database_statement.h>
#include <goose/parser/statement/pragma_statement.h>
#include <goose/parser/transformer.h>
#include <goose/parser/expression/constant_expression.h>

namespace goose {
    unique_ptr<SQLStatement> Transformer::TransformCopyDatabase(cantor::PGCopyDatabaseStmt &stmt) {
        if (stmt.copy_database_flag) {
            // copy a specific subset of the database
            CopyDatabaseType type;
            if (StringUtil::Equals(stmt.copy_database_flag, "schema")) {
                type = CopyDatabaseType::COPY_SCHEMA;
            } else if (StringUtil::Equals(stmt.copy_database_flag, "data")) {
                type = CopyDatabaseType::COPY_DATA;
            } else {
                throw NotImplementedException("Unsupported flag for COPY DATABASE");
            }
            return make_uniq<CopyDatabaseStatement>(stmt.from_database, stmt.to_database, type);
        } else {
            auto result = make_uniq<PragmaStatement>();
            result->info->name = "copy_database";
            result->info->parameters.emplace_back(make_uniq<ConstantExpression>(Value(stmt.from_database)));
            result->info->parameters.emplace_back(make_uniq<ConstantExpression>(Value(stmt.to_database)));
            return result;
        }
    }
} // namespace goose
