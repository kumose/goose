#include <goose/common/string_util.h>
#include <goose/common/types/value.h>
#include <goose/function/replacement_scan.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/expression/function_expression.h>
#include <goose/parser/statement/copy_statement.h>
#include <goose/parser/tableref/basetableref.h>
#include <goose/parser/transformer.h>

#include <cstring>

namespace goose {

void Transformer::TransformCopyOptions(CopyInfo &info, optional_ptr<cantor::PGList> options) {
	if (!options) {
		return;
	}

	cantor::PGListCell *cell;
	// iterate over each option
	for_each_cell(cell, options->head) {
		auto def_elem = PGPointerCast<cantor::PGDefElem>(cell->data.ptr_value);
		string name = def_elem->defname;
		if (info.parsed_options.find(name) != info.parsed_options.end()) {
			throw ParserException("Unexpected duplicate option \"%s\"", name);
		}
		unique_ptr<ParsedExpression> expr;
		if (def_elem->arg) {
			expr = TransformExpression(*def_elem->arg);
		}
		info.parsed_options[name] = std::move(expr);
	}
}

unique_ptr<CopyStatement> Transformer::TransformCopy(cantor::PGCopyStmt &stmt) {
	auto result = make_uniq<CopyStatement>();
	auto &info = *result->info;

	// get file_path and is_from
	info.is_from = stmt.is_from;

	info.file_path_expression = TransformExpression(*stmt.filename);
	if (info.file_path_expression->type == ExpressionType::VALUE_CONSTANT) {
		auto &constant_expr = info.file_path_expression->Cast<ConstantExpression>();
		if (constant_expr.value.type().id() == LogicalTypeId::VARCHAR) {
			info.file_path = StringValue::Get(constant_expr.value);
			info.file_path_expression.reset();
		}
	}

	// get select_list
	if (stmt.attlist) {
		for (auto n = stmt.attlist->head; n != nullptr; n = n->next) {
			auto target = PGPointerCast<cantor::PGResTarget>(n->data.ptr_value);
			if (target->name) {
				info.select_list.emplace_back(target->name);
			}
		}
	}

	if (stmt.relation) {
		auto ref = TransformRangeVar(*stmt.relation);
		auto &table = ref->Cast<BaseTableRef>();
		info.table = table.table_name;
		info.schema = table.schema_name;
		info.catalog = table.catalog_name;
	} else {
		info.select_statement = TransformSelectNode(*stmt.query);
	}

	// handle the different options of the COPY statement
	TransformCopyOptions(info, stmt.options);

	return result;
}

} // namespace goose
