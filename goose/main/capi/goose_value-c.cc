#include <goose/common/hugeint.h>
#include <goose/common/type_visitor.h>
#include <goose/common/types.h>
#include <goose/common/types/null_value.h>
#include <goose/common/types/string_type.h>
#include <goose/common/types/uuid.h>
#include <goose/common/types/value.h>
#include <goose/common/types/bignum.h>
#include <goose/main/capi/capi_internal.h>

using goose::LogicalTypeId;

static goose_value WrapValue(goose::Value *value) {
	return reinterpret_cast<goose_value>(value);
}

static goose::LogicalType &UnwrapType(goose_logical_type type) {
	return *(reinterpret_cast<goose::LogicalType *>(type));
}

static goose::Value &UnwrapValue(goose_value value) {
	return *(reinterpret_cast<goose::Value *>(value));
}
void goose_destroy_value(goose_value *value) {
	if (value && *value) {
		auto &unwrap_value = UnwrapValue(*value);
		delete &unwrap_value;
		*value = nullptr;
	}
}

goose_value goose_create_varchar_length(const char *text, idx_t length) {
	return WrapValue(new goose::Value(std::string(text, length)));
}

goose_value goose_create_varchar(const char *text) {
	return goose_create_varchar_length(text, strlen(text));
}

template <class T>
static goose_value CAPICreateValue(T input) {
	return WrapValue(new goose::Value(goose::Value::CreateValue<T>(input)));
}

template <class T, LogicalTypeId TYPE_ID>
static T CAPIGetValue(goose_value val) {
	auto &v = UnwrapValue(val);
	if (!v.DefaultTryCastAs(TYPE_ID)) {
		return goose::NullValue<T>();
	}
	return v.GetValue<T>();
}

goose_value goose_create_bool(bool input) {
	return CAPICreateValue(input);
}
bool goose_get_bool(goose_value val) {
	return CAPIGetValue<bool, LogicalTypeId::BOOLEAN>(val);
}
goose_value goose_create_int8(int8_t input) {
	return CAPICreateValue(input);
}
int8_t goose_get_int8(goose_value val) {
	return CAPIGetValue<int8_t, LogicalTypeId::TINYINT>(val);
}
goose_value goose_create_uint8(uint8_t input) {
	return CAPICreateValue(input);
}
uint8_t goose_get_uint8(goose_value val) {
	return CAPIGetValue<uint8_t, LogicalTypeId::UTINYINT>(val);
}
goose_value goose_create_int16(int16_t input) {
	return CAPICreateValue(input);
}
int16_t goose_get_int16(goose_value val) {
	return CAPIGetValue<int16_t, LogicalTypeId::SMALLINT>(val);
}
goose_value goose_create_uint16(uint16_t input) {
	return CAPICreateValue(input);
}
uint16_t goose_get_uint16(goose_value val) {
	return CAPIGetValue<uint16_t, LogicalTypeId::USMALLINT>(val);
}
goose_value goose_create_int32(int32_t input) {
	return CAPICreateValue(input);
}
int32_t goose_get_int32(goose_value val) {
	return CAPIGetValue<int32_t, LogicalTypeId::INTEGER>(val);
}
goose_value goose_create_uint32(uint32_t input) {
	return CAPICreateValue(input);
}
uint32_t goose_get_uint32(goose_value val) {
	return CAPIGetValue<uint32_t, LogicalTypeId::UINTEGER>(val);
}
goose_value goose_create_uint64(uint64_t input) {
	return CAPICreateValue(input);
}
uint64_t goose_get_uint64(goose_value val) {
	return CAPIGetValue<uint64_t, LogicalTypeId::UBIGINT>(val);
}
goose_value goose_create_int64(int64_t input) {
	return CAPICreateValue(input);
}
int64_t goose_get_int64(goose_value val) {
	return CAPIGetValue<int64_t, LogicalTypeId::BIGINT>(val);
}
goose_value goose_create_hugeint(goose_hugeint input) {
	return WrapValue(new goose::Value(goose::Value::HUGEINT(goose::hugeint_t(input.upper, input.lower))));
}
goose_hugeint goose_get_hugeint(goose_value val) {
	auto res = CAPIGetValue<goose::hugeint_t, LogicalTypeId::HUGEINT>(val);
	return {res.lower, res.upper};
}
goose_value goose_create_uhugeint(goose_uhugeint input) {
	return WrapValue(new goose::Value(goose::Value::UHUGEINT(goose::uhugeint_t(input.upper, input.lower))));
}
goose_uhugeint goose_get_uhugeint(goose_value val) {
	auto res = CAPIGetValue<goose::uhugeint_t, LogicalTypeId::UHUGEINT>(val);
	return {res.lower, res.upper};
}
goose_value goose_create_bignum(goose_bignum input) {
	return WrapValue(new goose::Value(
	    goose::Value::BIGNUM(goose::Bignum::FromByteArray(input.data, input.size, input.is_negative))));
}
goose_bignum goose_get_bignum(goose_value val) {
	auto v = UnwrapValue(val).DefaultCastAs(goose::LogicalType::BIGNUM);
	auto &str = goose::StringValue::Get(v);
	goose::vector<uint8_t> byte_array;
	bool is_negative;
	goose::Bignum::GetByteArray(byte_array, is_negative, goose::string_t(str));
	auto size = byte_array.size();
	auto data = reinterpret_cast<uint8_t *>(malloc(size));
	memcpy(data, byte_array.data(), size);
	return {data, size, is_negative};
}
goose_value goose_create_decimal(goose_decimal input) {
	goose::hugeint_t hugeint(input.value.upper, input.value.lower);
	int64_t int64;
	if (goose::Hugeint::TryCast<int64_t>(hugeint, int64)) {
		// The int64 DECIMAL value constructor will select the appropriate physical type based on width.
		return WrapValue(new goose::Value(goose::Value::DECIMAL(int64, input.width, input.scale)));
	} else {
		// The hugeint DECIMAL value constructor always uses a physical hugeint, and requires width >= MAX_WIDTH_INT64.
		return WrapValue(new goose::Value(goose::Value::DECIMAL(hugeint, input.width, input.scale)));
	}
}
goose_decimal goose_get_decimal(goose_value val) {
	auto &v = UnwrapValue(val);
	auto &type = v.type();
	if (type.id() != LogicalTypeId::DECIMAL) {
		return {0, 0, {0, 0}};
	}
	auto width = goose::DecimalType::GetWidth(type);
	auto scale = goose::DecimalType::GetScale(type);
	goose::hugeint_t hugeint = goose::IntegralValue::Get(v);
	return {width, scale, {hugeint.lower, hugeint.upper}};
}
goose_value goose_create_float(float input) {
	return CAPICreateValue(input);
}
float goose_get_float(goose_value val) {
	return CAPIGetValue<float, LogicalTypeId::FLOAT>(val);
}
goose_value goose_create_double(double input) {
	return CAPICreateValue(input);
}
double goose_get_double(goose_value val) {
	return CAPIGetValue<double, LogicalTypeId::DOUBLE>(val);
}
goose_value goose_create_date(goose_date input) {
	return CAPICreateValue(goose::date_t(input.days));
}
goose_date goose_get_date(goose_value val) {
	return {CAPIGetValue<goose::date_t, LogicalTypeId::DATE>(val).days};
}
goose_value goose_create_time(goose_time input) {
	return CAPICreateValue(goose::dtime_t(input.micros));
}
goose_time goose_get_time(goose_value val) {
	return {CAPIGetValue<goose::dtime_t, LogicalTypeId::TIME>(val).micros};
}
goose_value goose_create_time_tz_value(goose_time_tz input) {
	return CAPICreateValue(goose::dtime_tz_t(input.bits));
}
goose_time_tz goose_get_time_tz(goose_value val) {
	return {CAPIGetValue<goose::dtime_tz_t, LogicalTypeId::TIME_TZ>(val).bits};
}
goose_value goose_create_time_ns(goose_time_ns input) {
	return CAPICreateValue(goose::dtime_ns_t(input.nanos));
}
goose_time_ns goose_get_time_ns(goose_value val) {
	return {CAPIGetValue<goose::dtime_ns_t, LogicalTypeId::TIME_NS>(val).micros};
}

goose_value goose_create_timestamp(goose_timestamp input) {
	goose::timestamp_t ts(input.micros);
	return CAPICreateValue(ts);
}

goose_timestamp goose_get_timestamp(goose_value val) {
	if (!val) {
		return {0};
	}
	return {CAPIGetValue<goose::timestamp_t, LogicalTypeId::TIMESTAMP>(val).value};
}

goose_value goose_create_timestamp_tz(goose_timestamp input) {
	goose::timestamp_tz_t ts(input.micros);
	return CAPICreateValue(ts);
}

goose_timestamp goose_get_timestamp_tz(goose_value val) {
	if (!val) {
		return {0};
	}
	return {CAPIGetValue<goose::timestamp_tz_t, LogicalTypeId::TIMESTAMP_TZ>(val).value};
}

goose_value goose_create_timestamp_s(goose_timestamp_s input) {
	goose::timestamp_sec_t ts(input.seconds);
	return CAPICreateValue(ts);
}

goose_timestamp_s goose_get_timestamp_s(goose_value val) {
	if (!val) {
		return {0};
	}
	return {CAPIGetValue<goose::timestamp_sec_t, LogicalTypeId::TIMESTAMP_SEC>(val).value};
}

goose_value goose_create_timestamp_ms(goose_timestamp_ms input) {
	goose::timestamp_ms_t ts(input.millis);
	return CAPICreateValue(ts);
}

goose_timestamp_ms goose_get_timestamp_ms(goose_value val) {
	if (!val) {
		return {0};
	}
	return {CAPIGetValue<goose::timestamp_ms_t, LogicalTypeId::TIMESTAMP_MS>(val).value};
}

goose_value goose_create_timestamp_ns(goose_timestamp_ns input) {
	goose::timestamp_ns_t ts(input.nanos);
	return CAPICreateValue(ts);
}

goose_timestamp_ns goose_get_timestamp_ns(goose_value val) {
	if (!val) {
		return {0};
	}
	return {CAPIGetValue<goose::timestamp_ns_t, LogicalTypeId::TIMESTAMP_NS>(val).value};
}

goose_value goose_create_interval(goose_interval input) {
	return WrapValue(new goose::Value(goose::Value::INTERVAL(input.months, input.days, input.micros)));
}
goose_interval goose_get_interval(goose_value val) {
	auto interval = CAPIGetValue<goose::interval_t, LogicalTypeId::INTERVAL>(val);
	return {interval.months, interval.days, interval.micros};
}
goose_value goose_create_blob(const uint8_t *data, idx_t length) {
	return WrapValue(new goose::Value(goose::Value::BLOB((const uint8_t *)data, length)));
}
goose_blob goose_get_blob(goose_value val) {
	auto res = UnwrapValue(val).DefaultCastAs(goose::LogicalType::BLOB);
	auto &str = goose::StringValue::Get(res);

	auto result = reinterpret_cast<void *>(malloc(sizeof(char) * str.size()));
	memcpy(result, str.c_str(), str.size());
	return {result, str.size()};
}
goose_value goose_create_bit(goose_bit input) {
	return WrapValue(new goose::Value(goose::Value::BIT(input.data, input.size)));
}
goose_bit goose_get_bit(goose_value val) {
	auto v = UnwrapValue(val).DefaultCastAs(goose::LogicalType::BIT);
	auto &str = goose::StringValue::Get(v);
	auto size = str.size();
	auto data = reinterpret_cast<uint8_t *>(malloc(size));
	memcpy(data, str.c_str(), size);
	return {data, size};
}
goose_value goose_create_uuid(goose_uhugeint input) {
	// uhugeint_t has a constexpr ctor with upper first
	return WrapValue(new goose::Value(goose::Value::UUID(goose::UUID::FromUHugeint({input.upper, input.lower}))));
}
goose_uhugeint goose_get_uuid(goose_value val) {
	auto hugeint = CAPIGetValue<goose::hugeint_t, LogicalTypeId::UUID>(val);
	auto uhugeint = goose::UUID::ToUHugeint(hugeint);
	// goose_uhugeint has no constexpr ctor; struct is lower first
	return {uhugeint.lower, uhugeint.upper};
}

goose_logical_type goose_get_value_type(goose_value val) {
	auto &type = UnwrapValue(val).type();
	return (goose_logical_type)(&type);
}

char *goose_get_varchar(goose_value value) {
	auto val = reinterpret_cast<goose::Value *>(value);
	auto str_val = val->DefaultCastAs(goose::LogicalType::VARCHAR);
	auto &str = goose::StringValue::Get(str_val);

	auto result = reinterpret_cast<char *>(malloc(sizeof(char) * (str.size() + 1)));
	memcpy(result, str.c_str(), str.size());
	result[str.size()] = '\0';
	return result;
}
goose_value goose_create_struct_value(goose_logical_type type, goose_value *values) {
	if (!type || !values) {
		return nullptr;
	}
	const auto &logical_type = UnwrapType(type);
	if (logical_type.id() != goose::LogicalTypeId::STRUCT) {
		return nullptr;
	}
	if (goose::TypeVisitor::Contains(logical_type, goose::LogicalTypeId::INVALID) ||
	    goose::TypeVisitor::Contains(logical_type, goose::LogicalTypeId::ANY)) {
		return nullptr;
	}

	auto count = goose::StructType::GetChildCount(logical_type);
	goose::vector<goose::Value> unwrapped_values;
	for (idx_t i = 0; i < count; i++) {
		auto value = values[i];
		if (!value) {
			return nullptr;
		}
		unwrapped_values.emplace_back(UnwrapValue(value));
	}
	goose::Value *struct_value = new goose::Value;
	try {
		*struct_value = goose::Value::STRUCT(logical_type, std::move(unwrapped_values));
	} catch (...) {
		delete struct_value;
		return nullptr;
	}
	return WrapValue(struct_value);
}

goose_value goose_create_list_value(goose_logical_type type, goose_value *values, idx_t value_count) {
	if (!type || !values) {
		return nullptr;
	}
	auto &logical_type = UnwrapType(type);
	goose::vector<goose::Value> unwrapped_values;
	if (goose::TypeVisitor::Contains(logical_type, goose::LogicalTypeId::INVALID) ||
	    goose::TypeVisitor::Contains(logical_type, goose::LogicalTypeId::ANY)) {
		return nullptr;
	}

	for (idx_t i = 0; i < value_count; i++) {
		auto value = values[i];
		if (!value) {
			return nullptr;
		}
		unwrapped_values.push_back(UnwrapValue(value));
	}
	auto list_value = new goose::Value;
	try {
		*list_value = goose::Value::LIST(logical_type, std::move(unwrapped_values));
	} catch (...) {
		delete list_value;
		return nullptr;
	}
	return WrapValue(list_value);
}

goose_value goose_create_array_value(goose_logical_type type, goose_value *values, idx_t value_count) {
	if (!type || !values) {
		return nullptr;
	}
	if (value_count >= goose::ArrayType::MAX_ARRAY_SIZE) {
		return nullptr;
	}
	auto &logical_type = UnwrapType(type);
	if (goose::TypeVisitor::Contains(logical_type, goose::LogicalTypeId::INVALID) ||
	    goose::TypeVisitor::Contains(logical_type, goose::LogicalTypeId::ANY)) {
		return nullptr;
	}
	goose::vector<goose::Value> unwrapped_values;

	for (idx_t i = 0; i < value_count; i++) {
		auto value = values[i];
		if (!value) {
			return nullptr;
		}
		unwrapped_values.push_back(UnwrapValue(value));
	}
	goose::Value *array_value = new goose::Value;
	try {
		*array_value = goose::Value::ARRAY(logical_type, std::move(unwrapped_values));
	} catch (...) {
		delete array_value;
		return nullptr;
	}
	return WrapValue(array_value);
}

goose_value goose_create_map_value(goose_logical_type map_type, goose_value *keys, goose_value *values,
                                     idx_t entry_count) {
	if (!map_type || !keys || !values) {
		return nullptr;
	}
	const auto &map_logical_type = UnwrapType(map_type);
	if (map_logical_type.id() != goose::LogicalTypeId::MAP) {
		return nullptr;
	}
	if (goose::TypeVisitor::Contains(map_logical_type, goose::LogicalTypeId::INVALID) ||
	    goose::TypeVisitor::Contains(map_logical_type, goose::LogicalTypeId::ANY)) {
		return nullptr;
	}

	const auto &key_logical_type = goose::MapType::KeyType(map_logical_type);
	const auto &value_logical_type = goose::MapType::ValueType(map_logical_type);
	goose::vector<goose::Value> unwrapped_keys;
	goose::vector<goose::Value> unwrapped_values;
	for (idx_t i = 0; i < entry_count; i++) {
		const auto key = keys[i];
		const auto value = values[i];
		if (!key || !value) {
			return nullptr;
		}
		unwrapped_keys.emplace_back(UnwrapValue(key));
		unwrapped_values.emplace_back(UnwrapValue(value));
	}
	goose::Value *map_value = new goose::Value;
	try {
		*map_value = goose::Value::MAP(key_logical_type, value_logical_type, std::move(unwrapped_keys),
		                                std::move(unwrapped_values));
	} catch (...) {
		delete map_value;
		return nullptr;
	}
	return WrapValue(map_value);
}

goose_value goose_create_union_value(goose_logical_type union_type, idx_t tag_index, goose_value value) {
	if (!union_type || !value) {
		return nullptr;
	}
	const auto &union_logical_type = UnwrapType(union_type);
	if (union_logical_type.id() != goose::LogicalTypeId::UNION) {
		return nullptr;
	}
	idx_t member_count = goose::UnionType::GetMemberCount(union_logical_type);
	if (tag_index >= member_count) {
		return nullptr;
	}
	const auto &member_type = goose::UnionType::GetMemberType(union_logical_type, tag_index);
	const auto &unwrapped_value = UnwrapValue(value);
	if (unwrapped_value.type() != member_type) {
		return nullptr;
	}
	const auto member_types = goose::UnionType::CopyMemberTypes(union_logical_type);
	goose::Value *union_value = new goose::Value;
	try {
		*union_value = goose::Value::UNION(member_types, goose::NumericCast<uint8_t>(tag_index), unwrapped_value);
	} catch (...) {
		delete union_value;
		return nullptr;
	}
	return WrapValue(union_value);
}

idx_t goose_get_map_size(goose_value value) {
	if (!value) {
		return 0;
	}

	auto val = UnwrapValue(value);
	if (val.type().id() != LogicalTypeId::MAP || val.IsNull()) {
		return 0;
	}

	auto &children = goose::MapValue::GetChildren(val);
	return children.size();
}

goose_value goose_get_map_key(goose_value value, idx_t index) {
	if (!value) {
		return nullptr;
	}

	auto val = UnwrapValue(value);
	if (val.type().id() != LogicalTypeId::MAP || val.IsNull()) {
		return nullptr;
	}

	auto &children = goose::MapValue::GetChildren(val);
	if (index >= children.size()) {
		return nullptr;
	}

	auto &child = children[index];
	auto &child_struct = goose::StructValue::GetChildren(child);
	return WrapValue(new goose::Value(child_struct[0]));
}

goose_value goose_get_map_value(goose_value value, idx_t index) {
	if (!value) {
		return nullptr;
	}

	auto val = UnwrapValue(value);
	if (val.type().id() != LogicalTypeId::MAP || val.IsNull()) {
		return nullptr;
	}

	auto &children = goose::MapValue::GetChildren(val);
	if (index >= children.size()) {
		return nullptr;
	}

	auto &child = children[index];
	auto &child_struct = goose::StructValue::GetChildren(child);
	return WrapValue(new goose::Value(child_struct[1]));
}

bool goose_is_null_value(goose_value value) {
	if (!value) {
		return false;
	}
	return UnwrapValue(value).IsNull();
}

goose_value goose_create_null_value() {
	return WrapValue(new goose::Value());
}

idx_t goose_get_list_size(goose_value value) {
	if (!value) {
		return 0;
	}

	auto val = UnwrapValue(value);
	if (val.type().id() != LogicalTypeId::LIST || val.IsNull()) {
		return 0;
	}

	auto &children = goose::ListValue::GetChildren(val);
	return children.size();
}

goose_value goose_get_list_child(goose_value value, idx_t index) {
	if (!value) {
		return nullptr;
	}

	auto val = UnwrapValue(value);
	if (val.type().id() != LogicalTypeId::LIST || val.IsNull()) {
		return nullptr;
	}

	auto &children = goose::ListValue::GetChildren(val);
	if (index >= children.size()) {
		return nullptr;
	}

	return WrapValue(new goose::Value(children[index]));
}

goose_value goose_create_enum_value(goose_logical_type type, uint64_t value) {
	if (!type) {
		return nullptr;
	}

	auto &logical_type = UnwrapType(type);
	if (logical_type.id() != LogicalTypeId::ENUM) {
		return nullptr;
	}

	if (value >= goose::EnumType::GetSize(logical_type)) {
		return nullptr;
	}

	return WrapValue(new goose::Value(goose::Value::ENUM(value, logical_type)));
}

uint64_t goose_get_enum_value(goose_value value) {
	if (!value) {
		return 0;
	}

	auto val = UnwrapValue(value);
	if (val.type().id() != LogicalTypeId::ENUM || val.IsNull()) {
		return 0;
	}

	return val.GetValue<uint64_t>();
}

goose_value goose_get_struct_child(goose_value value, idx_t index) {
	if (!value) {
		return nullptr;
	}

	auto val = UnwrapValue(value);
	if (val.type().id() != LogicalTypeId::STRUCT || val.IsNull()) {
		return nullptr;
	}

	auto &children = goose::StructValue::GetChildren(val);
	if (index >= children.size()) {
		return nullptr;
	}

	return WrapValue(new goose::Value(children[index]));
}

char *goose_value_to_string(goose_value val) {
	if (!val) {
		return nullptr;
	}

	auto v = UnwrapValue(val);
	auto str = v.ToSQLString();

	auto result = reinterpret_cast<char *>(malloc(sizeof(char) * (str.size() + 1)));
	memcpy(result, str.c_str(), str.size());
	result[str.size()] = '\0';
	return result;
}
