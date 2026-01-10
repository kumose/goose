#include <goose/catalog/catalog_entry/table_column_type.h>
#include <goose/parser/constraint.h>
#include <goose/parser/expression/collate_expression.h>
#include <goose/parser/parsed_data/create_table_info.h>
#include <goose/parser/statement/create_statement.h>
#include <goose/parser/transformer.h>

namespace goose {

string Transformer::TransformCollation(optional_ptr<cantor::PGCollateClause> collate) {
	if (!collate) {
		return string();
	}
	string collation;
	for (auto c = collate->collname->head; c != nullptr; c = lnext(c)) {
		auto pgvalue = PGPointerCast<cantor::PGValue>(c->data.ptr_value);
		if (pgvalue->type != cantor::T_PGString) {
			throw ParserException("Expected a string as collation type!");
		}
		auto collation_argument = string(pgvalue->val.str);
		if (collation.empty()) {
			collation = collation_argument;
		} else {
			collation += "." + collation_argument;
		}
	}
	return collation;
}

OnCreateConflict Transformer::TransformOnConflict(cantor::PGOnCreateConflict conflict) {
	switch (conflict) {
	case cantor::PG_ERROR_ON_CONFLICT:
		return OnCreateConflict::ERROR_ON_CONFLICT;
	case cantor::PG_IGNORE_ON_CONFLICT:
		return OnCreateConflict::IGNORE_ON_CONFLICT;
	case cantor::PG_REPLACE_ON_CONFLICT:
		return OnCreateConflict::REPLACE_ON_CONFLICT;
	default:
		throw InternalException("Unrecognized OnConflict type");
	}
}

unique_ptr<ParsedExpression> Transformer::TransformCollateExpr(cantor::PGCollateClause &collate) {
	auto child = TransformExpression(collate.arg);
	auto collation = TransformCollation(&collate);
	return make_uniq<CollateExpression>(collation, std::move(child));
}

ColumnDefinition Transformer::TransformColumnDefinition(cantor::PGColumnDef &cdef) {
	string name;
	if (cdef.colname) {
		name = cdef.colname;
	}

	auto optional_type = cdef.category == cantor::COL_GENERATED;
	LogicalType target_type;
	if (optional_type && !cdef.typeName) {
		target_type = LogicalType::ANY;
	} else if (!cdef.typeName) {
		// ALTER TABLE tbl ALTER TYPE USING ...
		target_type = LogicalType::UNKNOWN;
	} else {
		target_type = TransformTypeName(*cdef.typeName);
	}

	if (cdef.collClause) {
		if (cdef.category == cantor::COL_GENERATED) {
			throw ParserException("Collations are not supported on generated columns");
		}
		if (target_type.id() != LogicalTypeId::VARCHAR) {
			throw ParserException("Only VARCHAR columns can have collations!");
		}
		target_type = LogicalType::VARCHAR_COLLATION(TransformCollation(cdef.collClause));
	}

	return ColumnDefinition(name, target_type);
}

unique_ptr<CreateStatement> Transformer::TransformCreateTable(cantor::PGCreateStmt &stmt) {
	auto result = make_uniq<CreateStatement>();
	auto info = make_uniq<CreateTableInfo>();

	if (stmt.inhRelations) {
		throw NotImplementedException("inherited relations not implemented");
	}
	D_ASSERT(stmt.relation);

	info->catalog = INVALID_CATALOG;
	auto qname = TransformQualifiedName(*stmt.relation);
	info->catalog = qname.catalog;
	info->schema = qname.schema;
	info->table = qname.name;
	info->on_conflict = TransformOnConflict(stmt.onconflict);
	info->temporary =
	    stmt.relation->relpersistence == cantor::PGPostgresRelPersistence::PG_RELPERSISTENCE_TEMP;

	if (info->temporary && stmt.oncommit != cantor::PGOnCommitAction::PG_ONCOMMIT_PRESERVE_ROWS &&
	    stmt.oncommit != cantor::PGOnCommitAction::PG_ONCOMMIT_NOOP) {
		throw NotImplementedException("Only ON COMMIT PRESERVE ROWS is supported");
	}
	if (!stmt.tableElts) {
		throw ParserException("Table must have at least one column!");
	}

	idx_t column_count = 0;
	for (auto c = stmt.tableElts->head; c != nullptr; c = lnext(c)) {
		auto node = PGPointerCast<cantor::PGNode>(c->data.ptr_value);
		switch (node->type) {
		case cantor::T_PGColumnDef: {
			auto pg_col_def = PGPointerCast<cantor::PGColumnDef>(c->data.ptr_value);
			auto col_def = TransformColumnDefinition(*pg_col_def);

			if (pg_col_def->constraints) {
				for (auto cell = pg_col_def->constraints->head; cell != nullptr; cell = cell->next) {
					auto pg_constraint = PGPointerCast<cantor::PGConstraint>(cell->data.ptr_value);
					auto constraint = TransformConstraint(*pg_constraint, col_def, info->columns.LogicalColumnCount());
					if (constraint) {
						info->constraints.push_back(std::move(constraint));
					}
				}
			}

			info->columns.AddColumn(std::move(col_def));
			column_count++;
			break;
		}
		case cantor::T_PGConstraint: {
			auto pg_constraint = PGPointerCast<cantor::PGConstraint>(c->data.ptr_value);
			info->constraints.push_back(TransformConstraint(*pg_constraint));
			break;
		}
		default:
			throw NotImplementedException("ColumnDef type not handled yet");
		}
	}

	if (!column_count) {
		throw ParserException("Table must have at least one column!");
	}

	result->info = std::move(info);
	return result;
}

} // namespace goose
