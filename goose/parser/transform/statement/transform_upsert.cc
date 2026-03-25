#include <goose/parser/statement/insert_statement.h>
#include <goose/parser/tableref/expressionlistref.h>
#include <goose/parser/transformer.h>

namespace goose {

OnConflictAction TransformOnConflictAction(cantor::PGOnConflictClause *on_conflict) {
	if (!on_conflict) {
		return OnConflictAction::THROW;
	}
	switch (on_conflict->action) {
	case cantor::PG_ONCONFLICT_NONE:
		return OnConflictAction::THROW;
	case cantor::PG_ONCONFLICT_NOTHING:
		return OnConflictAction::NOTHING;
	case cantor::PG_ONCONFLICT_UPDATE:
		return OnConflictAction::UPDATE;
	default:
		throw InternalException("Type not implemented for OnConflictAction");
	}
}

vector<string> Transformer::TransformConflictTarget(cantor::PGList &list) {
	vector<string> columns;
	for (auto cell = list.head; cell != nullptr; cell = cell->next) {
		auto index_element = PGPointerCast<cantor::PGIndexElem>(cell->data.ptr_value);
		if (index_element->collation) {
			throw NotImplementedException("Index with collation not supported yet!");
		}
		if (index_element->opclass) {
			throw NotImplementedException("Index with opclass not supported yet!");
		}
		if (!index_element->name) {
			throw NotImplementedException("Non-column index element not supported yet!");
		}
		if (index_element->nulls_ordering) {
			throw NotImplementedException("Index with null_ordering not supported yet!");
		}
		if (index_element->ordering) {
			throw NotImplementedException("Index with ordering not supported yet!");
		}
		columns.emplace_back(index_element->name);
	}
	return columns;
}

unique_ptr<OnConflictInfo> Transformer::DummyOnConflictClause(cantor::PGOnConflictActionAlias type,
                                                              const string &relname) {
	switch (type) {
	case cantor::PGOnConflictActionAlias::PG_ONCONFLICT_ALIAS_REPLACE: {
		// This can not be fully resolved yet until the bind stage
		auto result = make_uniq<OnConflictInfo>();
		result->action_type = OnConflictAction::REPLACE;
		return result;
	}
	case cantor::PGOnConflictActionAlias::PG_ONCONFLICT_ALIAS_IGNORE: {
		// We can just fully replace this with DO NOTHING, and be done with it
		auto result = make_uniq<OnConflictInfo>();
		result->action_type = OnConflictAction::NOTHING;
		return result;
	}
	default: {
		throw InternalException("Type not implemented for PGOnConflictActionAlias");
	}
	}
}

unique_ptr<OnConflictInfo> Transformer::TransformOnConflictClause(cantor::PGOnConflictClause *node,
                                                                  const string &) {
	auto stmt = PGPointerCast<cantor::PGOnConflictClause>(node);
	D_ASSERT(stmt);

	auto result = make_uniq<OnConflictInfo>();
	result->action_type = TransformOnConflictAction(stmt.get());

	if (stmt->infer) {
		// A filter for the ON CONFLICT ... is specified.
		if (!stmt->infer->indexElems) {
			throw NotImplementedException("ON CONSTRAINT conflict target is not supported yet");
		}
		result->indexed_columns = TransformConflictTarget(*stmt->infer->indexElems);
		if (stmt->infer->whereClause) {
			result->condition = TransformExpression(stmt->infer->whereClause);
		}
	}

	if (result->action_type == OnConflictAction::UPDATE) {
		result->set_info = TransformUpdateSetInfo(stmt->targetList, stmt->whereClause);
	}
	return result;
}

} // namespace goose
