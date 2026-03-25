#include <goose/parser/transformer.h>
#include <goose/parser/statement/detach_statement.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/common/string_util.h>

namespace goose {

unique_ptr<DetachStatement> Transformer::TransformDetach(cantor::PGDetachStmt &stmt) {
	auto result = make_uniq<DetachStatement>();
	auto info = make_uniq<DetachInfo>();
	info->name = stmt.db_name;
	info->if_not_found = TransformOnEntryNotFound(stmt.missing_ok);

	result->info = std::move(info);
	return result;
}

} // namespace goose
