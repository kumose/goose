#include <goose/parser/statement/create_statement.h>
#include <goose/parser/parsed_data/create_schema_info.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<CreateStatement> Transformer::TransformCreateSchema(cantor::PGCreateSchemaStmt &stmt) {
	auto result = make_uniq<CreateStatement>();
	auto info = make_uniq<CreateSchemaInfo>();

	D_ASSERT(stmt.schemaname);
	info->catalog = stmt.catalogname ? stmt.catalogname : INVALID_CATALOG;
	info->schema = stmt.schemaname;
	info->on_conflict = TransformOnConflict(stmt.onconflict);

	if (stmt.schemaElts) {
		// schema elements
		for (auto cell = stmt.schemaElts->head; cell != nullptr; cell = cell->next) {
			auto node = PGPointerCast<cantor::PGNode>(cell->data.ptr_value);
			switch (node->type) {
			case cantor::T_PGCreateStmt:
			case cantor::T_PGViewStmt:
			default:
				throw NotImplementedException("Schema element not supported yet!");
			}
		}
	}
	result->info = std::move(info);
	return result;
}

} // namespace goose
