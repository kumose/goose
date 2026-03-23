#include <goose/parser/parsed_data/create_type_info.h>
#include <goose/parser/statement/create_statement.h>
#include <goose/parser/transformer.h>
#include <goose/common/types.h>
#include <goose/common/types/vector.h>

namespace goose {

Vector Transformer::PGListToVector(optional_ptr<cantor::PGList> column_list, idx_t &size) {
	if (!column_list) {
		Vector result(LogicalType::VARCHAR);
		return result;
	}
	// First we discover the size of this list
	for (auto c = column_list->head; c != nullptr; c = lnext(c)) {
		size++;
	}

	Vector result(LogicalType::VARCHAR, size);
	auto result_ptr = FlatVector::GetData<string_t>(result);

	size = 0;
	for (auto c = column_list->head; c != nullptr; c = lnext(c)) {
		auto &type_val = *PGPointerCast<cantor::PGAConst>(c->data.ptr_value);
		auto &entry_value_node = type_val.val;
		if (entry_value_node.type != cantor::T_PGString) {
			throw ParserException("Expected a string constant as value");
		}

		auto entry_value = string(entry_value_node.val.str);
		result_ptr[size++] = StringVector::AddStringOrBlob(result, entry_value);
	}
	return result;
}

unique_ptr<CreateStatement> Transformer::TransformCreateType(cantor::PGCreateTypeStmt &stmt) {
	auto result = make_uniq<CreateStatement>();
	auto info = make_uniq<CreateTypeInfo>();

	auto qualified_name = TransformQualifiedName(*stmt.typeName);
	info->catalog = qualified_name.catalog;
	info->schema = qualified_name.schema;
	info->name = qualified_name.name;
	info->temporary = !stmt.typeName->relpersistence;
	info->on_conflict = TransformOnConflict(stmt.onconflict);

	switch (stmt.kind) {
	case cantor::PG_NEWTYPE_ENUM: {
		info->internal = false;
		if (stmt.query) {
			// CREATE TYPE mood AS ENUM (SELECT ...)
			D_ASSERT(stmt.vals == nullptr);
			auto query = TransformSelectStmt(*stmt.query, false);
			info->query = std::move(query);
			info->type = LogicalType::INVALID;
		} else {
			D_ASSERT(stmt.query == nullptr);
			idx_t size = 0;
			auto ordered_array = PGListToVector(stmt.vals, size);
			info->type = LogicalType::ENUM(ordered_array, size);
		}
	} break;

	case cantor::PG_NEWTYPE_ALIAS: {
		LogicalType target_type = TransformTypeName(*stmt.ofType);
		info->type = target_type;
	} break;

	default:
		throw InternalException("Unknown kind of new type");
	}
	result->info = std::move(info);
	return result;
}
} // namespace goose
