#include <goose/parser/transformer.h>
#include <goose/parser/statement/attach_statement.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/common/string_util.h>

namespace goose {

unique_ptr<AttachStatement> Transformer::TransformAttach(cantor::PGAttachStmt &stmt) {
	auto result = make_uniq<AttachStatement>();
	auto info = make_uniq<AttachInfo>();
	info->name = stmt.name ? stmt.name : string();
	info->path = stmt.path;
	info->on_conflict = TransformOnConflict(stmt.onconflict);

	if (stmt.options) {
		cantor::PGListCell *cell;
		for_each_cell(cell, stmt.options->head) {
			auto def_elem = PGPointerCast<cantor::PGDefElem>(cell->data.ptr_value);
			unique_ptr<ParsedExpression> expr;
			if (def_elem->arg) {
				expr = TransformExpression(def_elem->arg);
			} else {
				expr = make_uniq<ConstantExpression>(Value::BOOLEAN(true));
			}
			info->parsed_options[StringUtil::Lower(def_elem->defname)] = std::move(expr);
		}
	}
	result->info = std::move(info);
	return result;
}

} // namespace goose
