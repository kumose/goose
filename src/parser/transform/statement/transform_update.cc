#include <goose/parser/statement/update_extensions_statement.h>
#include <goose/parser/statement/update_statement.h>
#include <goose/parser/transformer.h>

namespace goose {

unique_ptr<UpdateSetInfo> Transformer::TransformUpdateSetInfo(cantor::PGList *target_list,
                                                              cantor::PGNode *where_clause) {
	auto result = make_uniq<UpdateSetInfo>();
	auto root = target_list;

	for (auto cell = root->head; cell != nullptr; cell = cell->next) {
		auto target = PGPointerCast<cantor::PGResTarget>(cell->data.ptr_value);
		if (target->indirection) {
			throw ParserException("Qualified column names in UPDATE .. SET not supported");
		}
		result->columns.emplace_back(target->name);
		result->expressions.push_back(TransformExpression(target->val));
	}

	result->condition = TransformExpression(where_clause);
	return result;
}

unique_ptr<UpdateStatement> Transformer::TransformUpdate(cantor::PGUpdateStmt &stmt) {
	auto result = make_uniq<UpdateStatement>();
	if (stmt.withClause) {
		auto with_clause = PGPointerCast<cantor::PGWithClause>(stmt.withClause);
		TransformCTE(*with_clause, result->cte_map);
	}

	result->table = TransformRangeVar(*stmt.relation);
	if (stmt.fromClause) {
		result->from_table = TransformFrom(stmt.fromClause);
	}
	result->set_info = TransformUpdateSetInfo(stmt.targetList, stmt.whereClause);

	// Grab and transform the returning columns from the parser.
	if (stmt.returningList) {
		TransformExpressionList(*stmt.returningList, result->returning_list);
	}
	return result;
}

unique_ptr<UpdateExtensionsStatement>
Transformer::TransformUpdateExtensions(cantor::PGUpdateExtensionsStmt &stmt) {
	auto result = make_uniq<UpdateExtensionsStatement>();
	auto info = make_uniq<UpdateExtensionsInfo>();

	if (stmt.extensions) {
		auto column_list = PGPointerCast<cantor::PGList>(stmt.extensions);
		for (auto c = column_list->head; c != nullptr; c = c->next) {
			auto value = PGPointerCast<cantor::PGValue>(c->data.ptr_value);
			info->extensions_to_update.emplace_back(value->val.str);
		}
	}

	result->info = std::move(info);
	return result;
}

} // namespace goose
