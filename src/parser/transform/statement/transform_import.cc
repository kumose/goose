#include <goose/parser/statement/pragma_statement.h>
#include <goose/parser/transformer.h>
#include <goose/parser/expression/constant_expression.h>

namespace goose {

unique_ptr<PragmaStatement> Transformer::TransformImport(cantor::PGImportStmt &stmt) {
	auto result = make_uniq<PragmaStatement>();
	result->info->name = "import_database";
	result->info->parameters.emplace_back(make_uniq<ConstantExpression>(Value(stmt.filename)));
	return result;
}

} // namespace goose
