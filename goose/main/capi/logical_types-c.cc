#include <goose/main/capi/capi_internal.h>
#include <goose/parser/parsed_data/create_type_info.h>
#include <goose/common/type_visitor.h>
#include <goose/common/helper.h>

namespace goose {

struct CCustomType {
	unique_ptr<LogicalType> base_type;
	string name;
};

} // namespace goose

static bool AssertLogicalTypeId(goose_logical_type type, goose::LogicalTypeId type_id) {
	if (!type) {
		return false;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	if (logical_type.id() != type_id) {
		return false;
	}
	return true;
}

static bool AssertInternalType(goose_logical_type type, goose::PhysicalType physical_type) {
	if (!type) {
		return false;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	if (logical_type.InternalType() != physical_type) {
		return false;
	}
	return true;
}

goose_logical_type goose_create_logical_type(goose_type type) {
	if (type == GOOSE_TYPE_DECIMAL || type == GOOSE_TYPE_ENUM || type == GOOSE_TYPE_LIST ||
	    type == GOOSE_TYPE_STRUCT || type == GOOSE_TYPE_MAP || type == GOOSE_TYPE_ARRAY ||
	    type == GOOSE_TYPE_UNION) {
		type = GOOSE_TYPE_INVALID;
	}

	auto type_id = goose::LogicalTypeIdFromC(type);
	return reinterpret_cast<goose_logical_type>(new goose::LogicalType(type_id));
}

goose_logical_type goose_create_list_type(goose_logical_type type) {
	if (!type) {
		return nullptr;
	}
	try {
		goose::LogicalType *logical_type =
		    new goose::LogicalType(goose::LogicalType::LIST(*reinterpret_cast<goose::LogicalType *>(type)));
		return reinterpret_cast<goose_logical_type>(logical_type);
	} catch (...) {
		return nullptr;
	}
}

goose_logical_type goose_create_array_type(goose_logical_type type, idx_t array_size) {
	if (!type) {
		return nullptr;
	}
	if (array_size >= goose::ArrayType::MAX_ARRAY_SIZE) {
		return nullptr;
	}
	try {
		goose::LogicalType *ltype = new goose::LogicalType(
		    goose::LogicalType::ARRAY(*reinterpret_cast<goose::LogicalType *>(type), array_size));
		return reinterpret_cast<goose_logical_type>(ltype);
	} catch (...) {
		return nullptr;
	}
}

goose_logical_type goose_create_union_type(goose_logical_type *member_types_p, const char **member_names,
                                             idx_t member_count) {
	if (!member_types_p || !member_names) {
		return nullptr;
	}
	goose::LogicalType **member_types = reinterpret_cast<goose::LogicalType **>(member_types_p);
	try {
		goose::child_list_t<goose::LogicalType> members;

		for (idx_t i = 0; i < member_count; i++) {
			members.push_back(make_pair(member_names[i], *member_types[i]));
		}
		goose::LogicalType *mtype = new goose::LogicalType(goose::LogicalType::UNION(members));
		return reinterpret_cast<goose_logical_type>(mtype);
	} catch (...) {
		return nullptr;
	}
}

goose_logical_type goose_create_struct_type(goose_logical_type *member_types_p, const char **member_names,
                                              idx_t member_count) {
	if (!member_types_p || !member_names) {
		return nullptr;
	}
	goose::LogicalType **member_types = (goose::LogicalType **)member_types_p;
	for (idx_t i = 0; i < member_count; i++) {
		if (!member_names[i] || !member_types[i]) {
			return nullptr;
		}
	}

	try {
		goose::child_list_t<goose::LogicalType> members;

		for (idx_t i = 0; i < member_count; i++) {
			members.push_back(make_pair(member_names[i], *member_types[i]));
		}
		goose::LogicalType *mtype = new goose::LogicalType(goose::LogicalType::STRUCT(members));
		return reinterpret_cast<goose_logical_type>(mtype);
	} catch (...) {
		return nullptr;
	}
}

goose_logical_type goose_create_enum_type(const char **member_names, idx_t member_count) {
	if (!member_names) {
		return nullptr;
	}
	goose::Vector enum_vector(goose::LogicalType::VARCHAR, member_count);
	auto enum_vector_ptr = goose::FlatVector::GetData<goose::string_t>(enum_vector);

	for (idx_t i = 0; i < member_count; i++) {
		if (!member_names[i]) {
			return nullptr;
		}
		enum_vector_ptr[i] = goose::StringVector::AddStringOrBlob(enum_vector, member_names[i]);
	}

	try {
		goose::LogicalType *mtype = new goose::LogicalType(goose::LogicalType::ENUM(enum_vector, member_count));
		return reinterpret_cast<goose_logical_type>(mtype);
	} catch (...) {
		return nullptr;
	}
}

goose_logical_type goose_create_map_type(goose_logical_type key_type, goose_logical_type value_type) {
	if (!key_type || !value_type) {
		return nullptr;
	}
	try {
		goose::LogicalType *mtype = new goose::LogicalType(goose::LogicalType::MAP(
		    *reinterpret_cast<goose::LogicalType *>(key_type), *reinterpret_cast<goose::LogicalType *>(value_type)));
		return reinterpret_cast<goose_logical_type>(mtype);
	} catch (...) {
		return nullptr;
	}
}

goose_logical_type goose_create_decimal_type(uint8_t width, uint8_t scale) {
	return reinterpret_cast<goose_logical_type>(new goose::LogicalType(goose::LogicalType::DECIMAL(width, scale)));
}

goose_type goose_get_type_id(goose_logical_type type) {
	if (!type) {
		return GOOSE_TYPE_INVALID;
	}
	auto logical_type = reinterpret_cast<goose::LogicalType *>(type);
	return goose::LogicalTypeIdToC(logical_type->id());
}

void goose_destroy_logical_type(goose_logical_type *type) {
	if (type && *type) {
		auto logical_type = reinterpret_cast<goose::LogicalType *>(*type);
		delete logical_type;
		*type = nullptr;
	}
}

uint8_t goose_decimal_width(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::DECIMAL)) {
		return 0;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	return goose::DecimalType::GetWidth(logical_type);
}

uint8_t goose_decimal_scale(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::DECIMAL)) {
		return 0;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	return goose::DecimalType::GetScale(logical_type);
}

goose_type goose_decimal_internal_type(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::DECIMAL)) {
		return GOOSE_TYPE_INVALID;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	switch (logical_type.InternalType()) {
	case goose::PhysicalType::INT16:
		return GOOSE_TYPE_SMALLINT;
	case goose::PhysicalType::INT32:
		return GOOSE_TYPE_INTEGER;
	case goose::PhysicalType::INT64:
		return GOOSE_TYPE_BIGINT;
	case goose::PhysicalType::INT128:
		return GOOSE_TYPE_HUGEINT;
	default:
		return GOOSE_TYPE_INVALID;
	}
}

goose_type goose_enum_internal_type(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::ENUM)) {
		return GOOSE_TYPE_INVALID;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	switch (logical_type.InternalType()) {
	case goose::PhysicalType::UINT8:
		return GOOSE_TYPE_UTINYINT;
	case goose::PhysicalType::UINT16:
		return GOOSE_TYPE_USMALLINT;
	case goose::PhysicalType::UINT32:
		return GOOSE_TYPE_UINTEGER;
	default:
		return GOOSE_TYPE_INVALID;
	}
}

uint32_t goose_enum_dictionary_size(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::ENUM)) {
		return 0;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	return goose::NumericCast<uint32_t>(goose::EnumType::GetSize(logical_type));
}

char *goose_enum_dictionary_value(goose_logical_type type, idx_t index) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::ENUM)) {
		return nullptr;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	auto &vector = goose::EnumType::GetValuesInsertOrder(logical_type);
	auto value = vector.GetValue(index);
	return strdup(goose::StringValue::Get(value).c_str());
}

goose_logical_type goose_list_type_child_type(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::LIST) &&
	    !AssertLogicalTypeId(type, goose::LogicalTypeId::MAP)) {
		return nullptr;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	if (logical_type.id() != goose::LogicalTypeId::LIST && logical_type.id() != goose::LogicalTypeId::MAP) {
		return nullptr;
	}
	return reinterpret_cast<goose_logical_type>(new goose::LogicalType(goose::ListType::GetChildType(logical_type)));
}

goose_logical_type goose_array_type_child_type(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::ARRAY)) {
		return nullptr;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	if (logical_type.id() != goose::LogicalTypeId::ARRAY) {
		return nullptr;
	}
	return reinterpret_cast<goose_logical_type>(
	    new goose::LogicalType(goose::ArrayType::GetChildType(logical_type)));
}

idx_t goose_array_type_array_size(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::ARRAY)) {
		return 0;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	if (logical_type.id() != goose::LogicalTypeId::ARRAY) {
		return 0;
	}
	return goose::ArrayType::GetSize(logical_type);
}

goose_logical_type goose_map_type_key_type(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::MAP)) {
		return nullptr;
	}
	auto &mtype = *(reinterpret_cast<goose::LogicalType *>(type));
	if (mtype.id() != goose::LogicalTypeId::MAP) {
		return nullptr;
	}
	return reinterpret_cast<goose_logical_type>(new goose::LogicalType(goose::MapType::KeyType(mtype)));
}

goose_logical_type goose_map_type_value_type(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::MAP)) {
		return nullptr;
	}
	auto &mtype = *(reinterpret_cast<goose::LogicalType *>(type));
	if (mtype.id() != goose::LogicalTypeId::MAP) {
		return nullptr;
	}
	return reinterpret_cast<goose_logical_type>(new goose::LogicalType(goose::MapType::ValueType(mtype)));
}

idx_t goose_struct_type_child_count(goose_logical_type type) {
	if (!AssertInternalType(type, goose::PhysicalType::STRUCT)) {
		return 0;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	return goose::StructType::GetChildCount(logical_type);
}

idx_t goose_union_type_member_count(goose_logical_type type) {
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::UNION)) {
		return 0;
	}
	idx_t member_count = goose_struct_type_child_count(type);
	if (member_count != 0) {
		member_count--;
	}
	return member_count;
}

char *goose_union_type_member_name(goose_logical_type type, idx_t index) {
	if (!AssertInternalType(type, goose::PhysicalType::STRUCT)) {
		return nullptr;
	}
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::UNION)) {
		return nullptr;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	return strdup(goose::UnionType::GetMemberName(logical_type, index).c_str());
}

goose_logical_type goose_union_type_member_type(goose_logical_type type, idx_t index) {
	if (!AssertInternalType(type, goose::PhysicalType::STRUCT)) {
		return nullptr;
	}
	if (!AssertLogicalTypeId(type, goose::LogicalTypeId::UNION)) {
		return nullptr;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	return reinterpret_cast<goose_logical_type>(
	    new goose::LogicalType(goose::UnionType::GetMemberType(logical_type, index)));
}

char *goose_struct_type_child_name(goose_logical_type type, idx_t index) {
	if (!AssertInternalType(type, goose::PhysicalType::STRUCT)) {
		return nullptr;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	return strdup(goose::StructType::GetChildName(logical_type, index).c_str());
}

char *goose_logical_type_get_alias(goose_logical_type type) {
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	return logical_type.HasAlias() ? strdup(logical_type.GetAlias().c_str()) : nullptr;
}

void goose_logical_type_set_alias(goose_logical_type type, const char *alias) {
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	logical_type.SetAlias(alias);
}

goose_logical_type goose_struct_type_child_type(goose_logical_type type, idx_t index) {
	if (!AssertInternalType(type, goose::PhysicalType::STRUCT)) {
		return nullptr;
	}
	auto &logical_type = *(reinterpret_cast<goose::LogicalType *>(type));
	if (logical_type.InternalType() != goose::PhysicalType::STRUCT) {
		return nullptr;
	}
	return reinterpret_cast<goose_logical_type>(
	    new goose::LogicalType(goose::StructType::GetChildType(logical_type, index)));
}

goose_state goose_register_logical_type(goose_connection connection, goose_logical_type type,
                                          goose_create_type_info info) {
	if (!connection || !type) {
		return GooseError;
	}

	// Unused for now
	(void)info;

	const auto &base_type = *reinterpret_cast<goose::LogicalType *>(type);
	if (!base_type.HasAlias()) {
		return GooseError;
	}

	if (goose::TypeVisitor::Contains(base_type, goose::LogicalTypeId::INVALID) ||
	    goose::TypeVisitor::Contains(base_type, goose::LogicalTypeId::ANY)) {
		return GooseError;
	}

	try {
		const auto con = reinterpret_cast<goose::Connection *>(connection);
		con->context->RunFunctionInTransaction([&]() {
			auto &catalog = goose::Catalog::GetSystemCatalog(*con->context);
			goose::CreateTypeInfo info(base_type.GetAlias(), base_type);
			info.temporary = true;
			info.internal = true;
			catalog.CreateType(*con->context, info);
		});
	} catch (...) {
		return GooseError;
	}
	return GooseSuccess;
}
