#include <goose/testing/capi_tester.h>

#include <goose/main/database_manager.h>
#include <goose/parser/parsed_data/create_type_info.h>
#include <goose/transaction/meta_transaction.h>

using namespace goose;
using namespace std;

TEST_CASE("Test decimal types C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	REQUIRE(tester.OpenDatabase(nullptr));

	// decimal types
	result =
	    tester.Query("SELECT 1.0::DECIMAL(4,1), 2.0::DECIMAL(9,2), 3.0::DECIMAL(18,3), 4.0::DECIMAL(38,4), 5::INTEGER");
	REQUIRE(NO_FAIL(*result));
	REQUIRE(result->ColumnCount() == 5);
	REQUIRE(result->ErrorMessage() == nullptr);

	if (goose_vector_size() < 64) {
		return;
	}

	// fetch the first chunk
	REQUIRE(result->ChunkCount() == 1);
	auto chunk = result->FetchChunk(0);
	REQUIRE(chunk);

	goose::vector<uint8_t> widths = {4, 9, 18, 38, 0};
	goose::vector<uint8_t> scales = {1, 2, 3, 4, 0};
	goose::vector<goose_type> types = {GOOSE_TYPE_DECIMAL, GOOSE_TYPE_DECIMAL, GOOSE_TYPE_DECIMAL,
	                                     GOOSE_TYPE_DECIMAL, GOOSE_TYPE_INTEGER};
	goose::vector<goose_type> internal_types = {GOOSE_TYPE_SMALLINT, GOOSE_TYPE_INTEGER, GOOSE_TYPE_BIGINT,
	                                              GOOSE_TYPE_HUGEINT, GOOSE_TYPE_INVALID};
	for (idx_t i = 0; i < result->ColumnCount(); i++) {
		auto logical_type = goose_vector_get_column_type(goose_data_chunk_get_vector(chunk->GetChunk(), i));
		REQUIRE(logical_type);
		REQUIRE(goose_get_type_id(logical_type) == types[i]);
		REQUIRE(goose_decimal_width(logical_type) == widths[i]);
		REQUIRE(goose_decimal_scale(logical_type) == scales[i]);
		REQUIRE(goose_decimal_internal_type(logical_type) == internal_types[i]);

		goose_destroy_logical_type(&logical_type);
	}
	REQUIRE(goose_decimal_width(nullptr) == 0);
	REQUIRE(goose_decimal_scale(nullptr) == 0);
	REQUIRE(goose_decimal_internal_type(nullptr) == GOOSE_TYPE_INVALID);
}

TEST_CASE("Test enum types C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	if (goose_vector_size() < 64) {
		return;
	}

	REQUIRE(tester.OpenDatabase(nullptr));
	result =
	    tester.Query("select small_enum, medium_enum, large_enum, int from test_all_types(use_large_enum = true);");
	REQUIRE(NO_FAIL(*result));
	REQUIRE(result->ColumnCount() == 4);
	REQUIRE(result->ErrorMessage() == nullptr);

	// fetch the first chunk
	REQUIRE(result->ChunkCount() == 1);
	auto chunk = result->FetchChunk(0);
	REQUIRE(chunk);

	goose::vector<goose_type> types = {GOOSE_TYPE_ENUM, GOOSE_TYPE_ENUM, GOOSE_TYPE_ENUM, GOOSE_TYPE_INTEGER};
	goose::vector<goose_type> internal_types = {GOOSE_TYPE_UTINYINT, GOOSE_TYPE_USMALLINT, GOOSE_TYPE_UINTEGER,
	                                              GOOSE_TYPE_INVALID};
	goose::vector<uint32_t> dictionary_sizes = {2, 300, 70000, 0};
	goose::vector<string> dictionary_strings = {"DUCK_DUCK_ENUM", "enum_0", "enum_0", string()};
	for (idx_t i = 0; i < result->ColumnCount(); i++) {
		auto logical_type = goose_vector_get_column_type(goose_data_chunk_get_vector(chunk->GetChunk(), i));
		REQUIRE(logical_type);
		REQUIRE(goose_get_type_id(logical_type) == types[i]);
		REQUIRE(goose_enum_internal_type(logical_type) == internal_types[i]);
		REQUIRE(goose_enum_dictionary_size(logical_type) == dictionary_sizes[i]);

		auto value = goose_enum_dictionary_value(logical_type, 0);
		string str_val = value ? string(value) : string();
		goose_free(value);

		REQUIRE(str_val == dictionary_strings[i]);

		goose_destroy_logical_type(&logical_type);
	}
	REQUIRE(goose_enum_internal_type(nullptr) == GOOSE_TYPE_INVALID);
	REQUIRE(goose_enum_dictionary_size(nullptr) == 0);
	REQUIRE(goose_enum_dictionary_value(nullptr, 0) == nullptr);
}

TEST_CASE("Test list types C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	REQUIRE(tester.OpenDatabase(nullptr));
	result = tester.Query("select [1, 2, 3] l, ['hello', 'world'] s, [[1, 2, 3], [4, 5]] nested, 3::int");
	REQUIRE(NO_FAIL(*result));
	REQUIRE(result->ColumnCount() == 4);
	REQUIRE(result->ErrorMessage() == nullptr);

	// fetch the first chunk
	REQUIRE(result->ChunkCount() == 1);
	auto chunk = result->FetchChunk(0);
	REQUIRE(chunk);

	goose::vector<goose_type> types = {GOOSE_TYPE_LIST, GOOSE_TYPE_LIST, GOOSE_TYPE_LIST, GOOSE_TYPE_INTEGER};
	goose::vector<goose_type> child_types_1 = {GOOSE_TYPE_INTEGER, GOOSE_TYPE_VARCHAR, GOOSE_TYPE_LIST,
	                                             GOOSE_TYPE_INVALID};
	goose::vector<goose_type> child_types_2 = {GOOSE_TYPE_INVALID, GOOSE_TYPE_INVALID, GOOSE_TYPE_INTEGER,
	                                             GOOSE_TYPE_INVALID};
	for (idx_t i = 0; i < result->ColumnCount(); i++) {
		auto logical_type = goose_vector_get_column_type(goose_data_chunk_get_vector(chunk->GetChunk(), i));
		REQUIRE(logical_type);
		REQUIRE(goose_get_type_id(logical_type) == types[i]);

		auto child_type = goose_list_type_child_type(logical_type);
		auto child_type_2 = goose_list_type_child_type(child_type);

		REQUIRE(goose_get_type_id(child_type) == child_types_1[i]);
		REQUIRE(goose_get_type_id(child_type_2) == child_types_2[i]);

		goose_destroy_logical_type(&child_type);
		goose_destroy_logical_type(&child_type_2);
		goose_destroy_logical_type(&logical_type);
	}
	REQUIRE(goose_list_type_child_type(nullptr) == nullptr);
}

TEST_CASE("Test struct types C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	REQUIRE(tester.OpenDatabase(nullptr));
	result = tester.Query("select {'a': 42::int}, {'b': 'hello', 'c': [1, 2, 3]}, {'d': {'e': 42}}, 3::int");
	REQUIRE(NO_FAIL(*result));
	REQUIRE(result->ColumnCount() == 4);
	REQUIRE(result->ErrorMessage() == nullptr);

	// fetch the first chunk
	REQUIRE(result->ChunkCount() == 1);
	auto chunk = result->FetchChunk(0);
	REQUIRE(chunk);

	goose::vector<goose_type> types = {GOOSE_TYPE_STRUCT, GOOSE_TYPE_STRUCT, GOOSE_TYPE_STRUCT,
	                                     GOOSE_TYPE_INTEGER};
	goose::vector<idx_t> child_count = {1, 2, 1, 0};
	goose::vector<goose::vector<string>> child_names = {{"a"}, {"b", "c"}, {"d"}, {}};
	goose::vector<goose::vector<goose_type>> child_types = {
	    {GOOSE_TYPE_INTEGER}, {GOOSE_TYPE_VARCHAR, GOOSE_TYPE_LIST}, {GOOSE_TYPE_STRUCT}, {}};
	for (idx_t i = 0; i < result->ColumnCount(); i++) {
		auto logical_type = goose_vector_get_column_type(goose_data_chunk_get_vector(chunk->GetChunk(), i));
		REQUIRE(logical_type);
		REQUIRE(goose_get_type_id(logical_type) == types[i]);

		REQUIRE(goose_struct_type_child_count(logical_type) == child_count[i]);
		for (idx_t c_idx = 0; c_idx < child_count[i]; c_idx++) {
			auto val = goose_struct_type_child_name(logical_type, c_idx);
			string str_val(val);
			goose_free(val);

			REQUIRE(child_names[i][c_idx] == str_val);

			auto child_type = goose_struct_type_child_type(logical_type, c_idx);
			REQUIRE(goose_get_type_id(child_type) == child_types[i][c_idx]);
			goose_destroy_logical_type(&child_type);
		}

		goose_destroy_logical_type(&logical_type);
	}
	REQUIRE(goose_struct_type_child_count(nullptr) == 0);
	REQUIRE(goose_struct_type_child_name(nullptr, 0) == nullptr);
	REQUIRE(goose_struct_type_child_type(nullptr, 0) == nullptr);
}

TEST_CASE("Test struct types creation C API", "[capi]") {
	goose::vector<goose_logical_type> types = {goose_create_logical_type(GOOSE_TYPE_INTEGER),
	                                             goose_create_logical_type(GOOSE_TYPE_VARCHAR)};
	goose::vector<const char *> names = {"a", "b"};

	auto logical_type = goose_create_struct_type(types.data(), names.data(), types.size());
	REQUIRE(goose_get_type_id(logical_type) == goose_type::GOOSE_TYPE_STRUCT);
	REQUIRE(goose_struct_type_child_count(logical_type) == 2);

	for (idx_t i = 0; i < names.size(); i++) {
		auto name = goose_struct_type_child_name(logical_type, i);
		string str_name(name);
		goose_free(name);
		REQUIRE(str_name == names[i]);

		auto type = goose_struct_type_child_type(logical_type, i);
		REQUIRE(goose_get_type_id(type) == goose_get_type_id(types[i]));
		goose_destroy_logical_type(&type);
	}

	for (idx_t i = 0; i < types.size(); i++) {
		goose_destroy_logical_type(&types[i]);
	}
	goose_destroy_logical_type(&logical_type);
}

TEST_CASE("Test enum types creation C API", "[capi]") {
	goose::vector<const char *> names = {"a", "b"};

	auto logical_type = goose_create_enum_type(names.data(), names.size());
	REQUIRE(goose_get_type_id(logical_type) == goose_type::GOOSE_TYPE_ENUM);
	REQUIRE(goose_enum_dictionary_size(logical_type) == 2);

	for (idx_t i = 0; i < names.size(); i++) {
		auto name = goose_enum_dictionary_value(logical_type, i);
		string str_name(name);
		goose_free(name);
		REQUIRE(str_name == names[i]);
	}
	goose_destroy_logical_type(&logical_type);

	REQUIRE(goose_create_enum_type(nullptr, 0) == nullptr);
}

TEST_CASE("Union type construction") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	goose::vector<goose_logical_type> member_types = {goose_create_logical_type(GOOSE_TYPE_VARCHAR),
	                                                    goose_create_logical_type(GOOSE_TYPE_INTEGER)};
	goose::vector<const char *> member_names = {"hello", "world"};

	auto res = goose_create_union_type(member_types.data(), member_names.data(), member_names.size());

	REQUIRE(goose_struct_type_child_count(res) == 3);

	auto get_id = [&](idx_t index) {
		auto typ = goose_union_type_member_type(res, index);
		auto id = goose_get_type_id(typ);
		goose_destroy_logical_type(&typ);
		return id;
	};
	auto get_name = [&](idx_t index) {
		auto name = goose_union_type_member_name(res, index);
		string name_s(name);
		goose_free(name);
		return name_s;
	};

	REQUIRE(get_id(0) == GOOSE_TYPE_VARCHAR);
	REQUIRE(get_id(1) == GOOSE_TYPE_INTEGER);

	REQUIRE(get_name(0) == "hello");
	REQUIRE(get_name(1) == "world");

	for (auto typ : member_types) {
		goose_destroy_logical_type(&typ);
	}
	goose_destroy_logical_type(&res);
}

TEST_CASE("Logical types with aliases", "[capi]") {
	CAPITester tester;

	REQUIRE(tester.OpenDatabase(nullptr));

	Connection *connection = reinterpret_cast<Connection *>(tester.connection);

	connection->BeginTransaction();

	child_list_t<LogicalType> children = {{"hello", LogicalType::VARCHAR}};
	auto id = LogicalType::STRUCT(children);
	auto type_name = "test_type";
	id.SetAlias(type_name);
	CreateTypeInfo info(type_name, id);

	auto &catalog_name = DatabaseManager::GetDefaultDatabase(*connection->context);
	auto &transaction = MetaTransaction::Get(*connection->context);
	auto &catalog = Catalog::GetCatalog(*connection->context, catalog_name);
	transaction.ModifyDatabase(catalog.GetAttached(), DatabaseModificationType::CREATE_CATALOG_ENTRY);
	catalog.CreateType(*connection->context, info);

	connection->Commit();

	auto result = tester.Query("SELECT {hello: 'world'}::test_type");

	REQUIRE(NO_FAIL(*result));

	auto chunk = result->FetchChunk(0);
	REQUIRE(chunk);

	for (idx_t i = 0; i < result->ColumnCount(); i++) {
		auto logical_type = goose_vector_get_column_type(goose_data_chunk_get_vector(chunk->GetChunk(), i));
		REQUIRE(logical_type);

		auto alias = goose_logical_type_get_alias(logical_type);
		REQUIRE(alias);
		REQUIRE(string(alias) == "test_type");
		goose_free(alias);

		goose_destroy_logical_type(&logical_type);
	}
}

TEST_CASE("goose_create_value", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

#define TEST_VALUE(creator, getter, expected)                                                                          \
	{                                                                                                                  \
		auto value = creator;                                                                                          \
		REQUIRE(getter(value) == expected);                                                                            \
		goose_destroy_value(&value);                                                                                  \
	}

#define TEST_NUMERIC_INTERNAL(creator, value)                                                                          \
	{                                                                                                                  \
		TEST_VALUE(creator, goose_get_int8, value);                                                                   \
		TEST_VALUE(creator, goose_get_uint8, value);                                                                  \
		TEST_VALUE(creator, goose_get_int16, value);                                                                  \
		TEST_VALUE(creator, goose_get_uint16, value);                                                                 \
		TEST_VALUE(creator, goose_get_int32, value);                                                                  \
		TEST_VALUE(creator, goose_get_uint32, value);                                                                 \
		TEST_VALUE(creator, goose_get_int64, value);                                                                  \
		TEST_VALUE(creator, goose_get_uint64, value);                                                                 \
		TEST_VALUE(creator, goose_get_float, value);                                                                  \
		TEST_VALUE(creator, goose_get_double, value);                                                                 \
	}

#define TEST_NUMERIC(value)                                                                                            \
	{                                                                                                                  \
		TEST_NUMERIC_INTERNAL(goose_create_int8(value), value);                                                       \
		TEST_NUMERIC_INTERNAL(goose_create_uint8(value), value);                                                      \
		TEST_NUMERIC_INTERNAL(goose_create_int16(value), value);                                                      \
		TEST_NUMERIC_INTERNAL(goose_create_uint16(value), value);                                                     \
		TEST_NUMERIC_INTERNAL(goose_create_int32(value), value);                                                      \
		TEST_NUMERIC_INTERNAL(goose_create_uint32(value), value);                                                     \
		TEST_NUMERIC_INTERNAL(goose_create_int64(value), value);                                                      \
		TEST_NUMERIC_INTERNAL(goose_create_uint64(value), value);                                                     \
		TEST_NUMERIC_INTERNAL(goose_create_float(value), value);                                                      \
		TEST_NUMERIC_INTERNAL(goose_create_double(value), value);                                                     \
	}

	TEST_VALUE(goose_create_bool(true), goose_get_bool, true);
	TEST_NUMERIC(42);

	{
		auto val = goose_create_hugeint({42, 42});
		auto result = goose_get_hugeint(val);
		REQUIRE(result.lower == 42);
		REQUIRE(result.upper == 42);
		goose_destroy_value(&val);
	}
	{
		auto val = goose_create_uhugeint({42, 42});
		auto result = goose_get_uhugeint(val);
		REQUIRE(result.lower == 42);
		REQUIRE(result.upper == 42);
		goose_destroy_value(&val);
	}

	TEST_VALUE(goose_create_float(0.5), goose_get_float, 0.5);
	TEST_VALUE(goose_create_float(0.5), goose_get_double, 0.5);
	TEST_VALUE(goose_create_double(0.5), goose_get_double, 0.5);

	{
		auto val = goose_create_date({1});
		auto result = goose_get_date(val);
		REQUIRE(result.days == 1);
		// conversion failure (date -> numeric)
		REQUIRE(goose_get_int8(val) == NumericLimits<int8_t>::Minimum());
		REQUIRE(goose_get_uint8(val) == NumericLimits<uint8_t>::Minimum());
		REQUIRE(goose_get_int16(val) == NumericLimits<int16_t>::Minimum());
		REQUIRE(goose_get_uint16(val) == NumericLimits<uint16_t>::Minimum());
		REQUIRE(goose_get_int32(val) == NumericLimits<int32_t>::Minimum());
		REQUIRE(goose_get_uint32(val) == NumericLimits<uint32_t>::Minimum());
		REQUIRE(goose_get_int64(val) == NumericLimits<int64_t>::Minimum());
		REQUIRE(goose_get_uint64(val) == NumericLimits<uint64_t>::Minimum());
		REQUIRE(std::isnan(goose_get_float(val)));
		REQUIRE(std::isnan(goose_get_double(val)));
		auto min_hugeint = goose_get_hugeint(val);
		REQUIRE(min_hugeint.lower == NumericLimits<uint64_t>::Minimum());
		REQUIRE(min_hugeint.upper == NumericLimits<int64_t>::Minimum());
		auto min_uhugeint = goose_get_uhugeint(val);
		REQUIRE(min_uhugeint.lower == NumericLimits<uint64_t>::Minimum());
		REQUIRE(min_uhugeint.upper == NumericLimits<uint64_t>::Minimum());
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_time({1});
		auto result = goose_get_time(val);
		REQUIRE(result.micros == 1);
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_time_ns({1});
		auto result = goose_get_time_ns(val);
		REQUIRE(result.nanos == 1);
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_timestamp({1});
		REQUIRE(goose_get_timestamp(nullptr).micros == 0);
		auto result = goose_get_timestamp(val);
		REQUIRE(result.micros == 1);
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_timestamp_tz({1});
		REQUIRE(goose_get_timestamp_tz(nullptr).micros == 0);
		auto result = goose_get_timestamp_tz(val);
		REQUIRE(result.micros == 1);
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_timestamp_s({1});
		REQUIRE(goose_get_timestamp_s(nullptr).seconds == 0);
		auto result = goose_get_timestamp_s(val);
		REQUIRE(result.seconds == 1);
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_timestamp_ms({1});
		REQUIRE(goose_get_timestamp_ms(nullptr).millis == 0);
		auto result = goose_get_timestamp_ms(val);
		REQUIRE(result.millis == 1);
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_timestamp_ns({1});
		REQUIRE(goose_get_timestamp_ns(nullptr).nanos == 0);
		auto result = goose_get_timestamp_ns(val);
		REQUIRE(result.nanos == 1);
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_interval({1, 1, 1});
		auto result = goose_get_interval(val);
		REQUIRE(result.months == 1);
		REQUIRE(result.days == 1);
		REQUIRE(result.micros == 1);
		REQUIRE(result.days == 1);
		goose_destroy_value(&val);
	}
	{
		auto val = goose_create_blob((const uint8_t *)"hello", 5);
		auto result = goose_get_blob(val);
		REQUIRE(result.size == 5);
		REQUIRE(memcmp(result.data, "hello", 5) == 0);
		goose_free(result.data);
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_time_tz_value({1});
		auto result = goose_get_time_tz(val);
		REQUIRE(result.bits == 1);
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_bool(true);
		auto result = goose_get_value_type(val);
		REQUIRE(goose_get_type_id(result) == GOOSE_TYPE_BOOLEAN);
		goose_destroy_value(&val);
	}

	{
		auto val = goose_create_varchar("hello");
		auto result = goose_get_varchar(val);
		REQUIRE(string(result) == "hello");
		goose_free(result);
		goose_destroy_value(&val);
	}
}

TEST_CASE("Statement types", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	goose_prepared_statement prepared;
	REQUIRE(goose_prepare(tester.connection, "select ?", &prepared) == GooseSuccess);

	REQUIRE(goose_prepared_statement_type(prepared) == GOOSE_STATEMENT_TYPE_SELECT);
	goose_destroy_prepare(&prepared);

	auto result = tester.Query("CREATE TABLE t1 (id int)");

	REQUIRE(goose_result_statement_type(result->InternalResult()) == GOOSE_STATEMENT_TYPE_CREATE);
}

TEST_CASE("Constructing values", "[capi]") {
	std::vector<const char *> member_names {"hello", "world"};
	goose::vector<goose_type> child_types = {GOOSE_TYPE_INTEGER};
	auto first_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	auto second_type = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
	goose::vector<goose_logical_type> member_types = {first_type, second_type};
	auto struct_type = goose_create_struct_type(member_types.data(), member_names.data(), member_names.size());

	auto value = goose_create_int64(42);
	auto other_value = goose_create_varchar("other value");
	goose::vector<goose_value> struct_values {other_value, value};
	auto struct_value = goose_create_struct_value(struct_type, struct_values.data());
	REQUIRE(struct_value == nullptr);
	goose_destroy_logical_type(&struct_type);

	goose::vector<goose_value> list_values {value, other_value};
	auto list_value = goose_create_list_value(first_type, list_values.data(), list_values.size());
	REQUIRE(list_value == nullptr);

	goose_destroy_value(&value);
	goose_destroy_value(&other_value);
	goose_destroy_logical_type(&first_type);
	goose_destroy_logical_type(&second_type);
	goose_destroy_value(&list_value);
}

TEST_CASE("Binding values", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	goose_prepared_statement prepared;
	REQUIRE(goose_prepare(tester.connection, "SELECT ?, ?", &prepared) == GooseSuccess);

	std::vector<const char *> member_names {"hello"};
	goose::vector<goose_type> child_types = {GOOSE_TYPE_INTEGER};
	auto member_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	idx_t member_count = member_names.size();
	auto struct_type = goose_create_struct_type(&member_type, member_names.data(), member_count);

	auto value = goose_create_int64(42);
	auto struct_value = goose_create_struct_value(struct_type, &value);
	goose_destroy_logical_type(&struct_type);

	// Fail with out-of-bounds.
	goose_prepared_statement prepared_fail;
	REQUIRE(goose_prepare(tester.connection, "SELECT ?, ?", &prepared_fail) == GooseSuccess);
	auto state = goose_bind_value(prepared_fail, 3, struct_value);
	REQUIRE(state == GooseError);
	auto error_msg = goose_prepare_error(prepared_fail);
	REQUIRE(StringUtil::Contains(string(error_msg), "Can not bind to parameter number"));
	goose_destroy_prepare(&prepared_fail);

	goose::vector<goose_value> list_values {value};
	auto list_value = goose_create_list_value(member_type, list_values.data(), member_count);

	goose_destroy_value(&value);
	goose_destroy_logical_type(&member_type);

	goose_bind_value(prepared, 1, struct_value);
	goose_bind_value(prepared, 2, list_value);

	goose_destroy_value(&struct_value);
	goose_destroy_value(&list_value);

	auto result = tester.QueryPrepared(prepared);
	goose_destroy_prepare(&prepared);

	REQUIRE(result->ErrorMessage() == nullptr);
	REQUIRE(result->ColumnCount() == 2);

	// fetch the first chunk
	REQUIRE(result->ChunkCount() == 1);
	auto chunk = result->FetchChunk(0);
	REQUIRE(chunk);
	for (idx_t i = 0; i < result->ColumnCount(); i++) {
		goose_data_chunk current_chunk = chunk->GetChunk();
		auto current_vector = goose_data_chunk_get_vector(current_chunk, i);
		auto logical_type = goose_vector_get_column_type(current_vector);
		REQUIRE(logical_type);
		auto type_id = goose_get_type_id(logical_type);

		for (idx_t c_idx = 0; c_idx < member_count; c_idx++) {
			if (type_id == GOOSE_TYPE_STRUCT) {
				auto val = goose_struct_type_child_name(logical_type, c_idx);
				string str_val(val);
				goose_free(val);

				REQUIRE(member_names[c_idx] == str_val);
				auto child_type = goose_struct_type_child_type(logical_type, c_idx);
				REQUIRE(goose_get_type_id(child_type) == GOOSE_TYPE_INTEGER);
				goose_destroy_logical_type(&child_type);

				auto int32_vector = goose_struct_vector_get_child(current_vector, i);

				auto int32_data = (int32_t *)goose_vector_get_data(int32_vector);

				REQUIRE(int32_data[0] == 42);

			} else if (type_id == GOOSE_TYPE_LIST) {
				auto child_type = goose_list_type_child_type(logical_type);
				REQUIRE(goose_get_type_id(child_type) == GOOSE_TYPE_INTEGER);
				goose_destroy_logical_type(&child_type);

				REQUIRE(goose_list_vector_get_size(current_vector) == 1);
				auto int32_vector = goose_list_vector_get_child(current_vector);

				auto int32_data = (int32_t *)goose_vector_get_data(int32_vector);

				REQUIRE(int32_data[0] == 42);

			} else {
				FAIL();
			}
		}

		goose_destroy_logical_type(&logical_type);
	}
}

TEST_CASE("Test Infinite Dates", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	{
		auto result = tester.Query("SELECT '-infinity'::DATE, 'epoch'::DATE, 'infinity'::DATE");
		REQUIRE(NO_FAIL(*result));
		REQUIRE(result->ColumnCount() == 3);
		REQUIRE(result->ErrorMessage() == nullptr);

		auto d = result->Fetch<goose_date>(0, 0);
		REQUIRE(!goose_is_finite_date(d));
		REQUIRE(d.days < 0);

		d = result->Fetch<goose_date>(1, 0);
		REQUIRE(goose_is_finite_date(d));
		REQUIRE(d.days == 0);

		d = result->Fetch<goose_date>(2, 0);
		REQUIRE(!goose_is_finite_date(d));
		REQUIRE(d.days > 0);
	}

	{
		auto result = tester.Query("SELECT '-infinity'::TIMESTAMP, 'epoch'::TIMESTAMP, 'infinity'::TIMESTAMP");
		REQUIRE(NO_FAIL(*result));
		REQUIRE(result->ColumnCount() == 3);
		REQUIRE(result->ErrorMessage() == nullptr);

		auto ts = result->Fetch<goose_timestamp>(0, 0);
		REQUIRE(!goose_is_finite_timestamp(ts));
		REQUIRE(ts.micros < 0);

		ts = result->Fetch<goose_timestamp>(1, 0);
		REQUIRE(goose_is_finite_timestamp(ts));
		REQUIRE(ts.micros == 0);

		ts = result->Fetch<goose_timestamp>(2, 0);
		REQUIRE(!goose_is_finite_timestamp(ts));
		REQUIRE(ts.micros > 0);
	}

	{
		auto result = tester.Query("SELECT '-infinity'::TIMESTAMPTZ, 'epoch'::TIMESTAMPTZ, 'infinity'::TIMESTAMPTZ");
		REQUIRE(NO_FAIL(*result));
		REQUIRE(result->ColumnCount() == 3);
		REQUIRE(result->ErrorMessage() == nullptr);

		auto ts = result->Fetch<goose_timestamp>(0, 0);
		REQUIRE(!goose_is_finite_timestamp(ts));
		REQUIRE(ts.micros < 0);

		ts = result->Fetch<goose_timestamp>(1, 0);
		REQUIRE(goose_is_finite_timestamp(ts));
		REQUIRE(ts.micros == 0);

		ts = result->Fetch<goose_timestamp>(2, 0);
		REQUIRE(!goose_is_finite_timestamp(ts));
		REQUIRE(ts.micros > 0);
	}

	{
		auto result = tester.Query("SELECT '-infinity'::TIMESTAMP_S, 'epoch'::TIMESTAMP_S, 'infinity'::TIMESTAMP_S");
		REQUIRE(NO_FAIL(*result));
		REQUIRE(result->ColumnCount() == 3);
		REQUIRE(result->ErrorMessage() == nullptr);

		auto ts = result->Fetch<goose_timestamp_s>(0, 0);
		REQUIRE(!goose_is_finite_timestamp_s(ts));
		REQUIRE(ts.seconds < 0);

		ts = result->Fetch<goose_timestamp_s>(1, 0);
		REQUIRE(goose_is_finite_timestamp_s(ts));
		REQUIRE(ts.seconds == 0);

		ts = result->Fetch<goose_timestamp_s>(2, 0);
		REQUIRE(!goose_is_finite_timestamp_s(ts));
		REQUIRE(ts.seconds > 0);
	}

	{
		auto result = tester.Query("SELECT '-infinity'::TIMESTAMP_MS, 'epoch'::TIMESTAMP_MS, 'infinity'::TIMESTAMP_MS");
		REQUIRE(NO_FAIL(*result));
		REQUIRE(result->ColumnCount() == 3);
		REQUIRE(result->ErrorMessage() == nullptr);

		auto ts = result->Fetch<goose_timestamp_ms>(0, 0);
		REQUIRE(!goose_is_finite_timestamp_ms(ts));
		REQUIRE(ts.millis < 0);

		ts = result->Fetch<goose_timestamp_ms>(1, 0);
		REQUIRE(goose_is_finite_timestamp_ms(ts));
		REQUIRE(ts.millis == 0);

		ts = result->Fetch<goose_timestamp_ms>(2, 0);
		REQUIRE(!goose_is_finite_timestamp_ms(ts));
		REQUIRE(ts.millis > 0);
	}

	{
		auto result = tester.Query("SELECT '-infinity'::TIMESTAMP_NS, 'epoch'::TIMESTAMP_NS, 'infinity'::TIMESTAMP_NS");
		REQUIRE(NO_FAIL(*result));
		REQUIRE(result->ColumnCount() == 3);
		REQUIRE(result->ErrorMessage() == nullptr);

		auto ts = result->Fetch<goose_timestamp_ns>(0, 0);
		REQUIRE(!goose_is_finite_timestamp_ns(ts));
		REQUIRE(ts.nanos < 0);

		ts = result->Fetch<goose_timestamp_ns>(1, 0);
		REQUIRE(goose_is_finite_timestamp_ns(ts));
		REQUIRE(ts.nanos == 0);

		ts = result->Fetch<goose_timestamp_ns>(2, 0);
		REQUIRE(!goose_is_finite_timestamp_ns(ts));
		REQUIRE(ts.nanos > 0);
	}
}

TEST_CASE("Array type construction") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	auto child_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	auto array_type = goose_create_array_type(child_type, 3);

	REQUIRE(goose_array_type_array_size(array_type) == 3);

	auto get_child_type = goose_array_type_child_type(array_type);
	REQUIRE(goose_get_type_id(get_child_type) == GOOSE_TYPE_INTEGER);
	goose_destroy_logical_type(&get_child_type);

	goose_destroy_logical_type(&child_type);
	goose_destroy_logical_type(&array_type);
}

TEST_CASE("Array value construction") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	auto child_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);

	goose::vector<goose_value> values;
	values.push_back(goose_create_int64(42));
	values.push_back(goose_create_int64(43));
	values.push_back(goose_create_int64(44));

	auto array_value = goose_create_array_value(child_type, values.data(), values.size());
	REQUIRE(array_value);

	goose_destroy_logical_type(&child_type);
	for (auto &val : values) {
		goose_destroy_value(&val);
	}
	goose_destroy_value(&array_value);
}

TEST_CASE("Map value construction (happy path)", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	auto key_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	auto value_type = goose_create_logical_type(GOOSE_TYPE_DOUBLE);
	auto map_type = goose_create_map_type(key_type, value_type);

	int32_t keys[] {100, 200, 300};
	double values[] {1.2, 3.4, 5.6};
	idx_t entry_count = 3;

	goose::vector<goose_value> key_vals;
	goose::vector<goose_value> value_vals;
	for (idx_t i = 0; i < entry_count; ++i) {
		key_vals.push_back(goose_create_int32(keys[i]));
		value_vals.push_back(goose_create_double(values[i]));
	}

	auto map_value = goose_create_map_value(map_type, key_vals.data(), value_vals.data(), entry_count);
	REQUIRE(map_value);
	REQUIRE(goose_get_map_size(map_value) == entry_count);
	for (idx_t i = 0; i < entry_count; ++i) {
		auto key_val = goose_get_map_key(map_value, i);
		REQUIRE(goose_get_int32(key_val) == keys[i]);
		goose_destroy_value(&key_val);
		auto value_val = goose_get_map_value(map_value, i);
		REQUIRE(goose_get_double(value_val) == values[i]);
		goose_destroy_value(&value_val);
	}

	goose_prepared_statement prepared;
	REQUIRE(goose_prepare(tester.connection, "select ?", &prepared) == GooseSuccess);

	goose_bind_value(prepared, 1, map_value);

	auto result = tester.QueryPrepared(prepared);
	REQUIRE(result->ChunkCount() == 1);
	auto capi_chunk = result->FetchChunk(0);
	REQUIRE(capi_chunk);
	auto chunk = capi_chunk->GetChunk();
	REQUIRE(chunk);
	auto row_count = goose_data_chunk_get_size(chunk);
	REQUIRE(row_count == 1);
	auto vector = goose_data_chunk_get_vector(chunk, 0);
	REQUIRE(vector);
	auto logical_type = goose_vector_get_column_type(vector);
	REQUIRE(logical_type);
	REQUIRE(goose_get_type_id(logical_type) == goose_type::GOOSE_TYPE_MAP);
	goose_destroy_logical_type(&logical_type);

	auto list_entry_data = (uint64_t *)goose_vector_get_data(vector);
	REQUIRE(list_entry_data);
	REQUIRE(list_entry_data[0] == 0);           // offset
	REQUIRE(list_entry_data[1] == entry_count); // length
	REQUIRE(goose_list_vector_get_size(vector) == entry_count);
	auto list_child = goose_list_vector_get_child(vector);
	REQUIRE(list_child);
	auto keys_vector = goose_struct_vector_get_child(list_child, 0);
	REQUIRE(keys_vector);
	auto values_vector = goose_struct_vector_get_child(list_child, 1);
	REQUIRE(values_vector);
	auto keys_data = (int32_t *)goose_vector_get_data(keys_vector);
	REQUIRE(keys_data);
	auto values_data = (double *)goose_vector_get_data(values_vector);
	REQUIRE(values_data);
	for (idx_t i = 0; i < entry_count; ++i) {
		REQUIRE(keys_data[i] == keys[i]);
		REQUIRE(values_data[i] == values[i]);
	}

	goose_destroy_prepare(&prepared);
	goose_destroy_value(&map_value);
	for (idx_t i = 0; i < entry_count; ++i) {
		goose_destroy_value(&key_vals[i]);
		goose_destroy_value(&value_vals[i]);
	}
	goose_destroy_logical_type(&map_type);
	goose_destroy_logical_type(&value_type);
	goose_destroy_logical_type(&key_type);
}

TEST_CASE("Map value construction (null map type)", "[capi]") {
	goose::vector<goose_value> key_vals;
	goose::vector<goose_value> value_vals;
	auto map_value = goose_create_map_value(nullptr, key_vals.data(), value_vals.data(), 0);
	REQUIRE(map_value == nullptr);
}

TEST_CASE("Map value construction (null keys array)", "[capi]") {
	auto key_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	auto value_type = goose_create_logical_type(GOOSE_TYPE_DOUBLE);
	auto map_type = goose_create_map_type(key_type, value_type);
	goose::vector<goose_value> value_vals;
	auto map_value = goose_create_map_value(map_type, nullptr, value_vals.data(), 0);
	REQUIRE(map_value == nullptr);
	goose_destroy_logical_type(&map_type);
	goose_destroy_logical_type(&value_type);
	goose_destroy_logical_type(&key_type);
}

TEST_CASE("Map value construction (null values array)", "[capi]") {
	auto key_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	auto value_type = goose_create_logical_type(GOOSE_TYPE_DOUBLE);
	auto map_type = goose_create_map_type(key_type, value_type);
	goose::vector<goose_value> key_vals;
	auto map_value = goose_create_map_value(map_type, key_vals.data(), nullptr, 0);
	REQUIRE(map_value == nullptr);
	goose_destroy_logical_type(&map_type);
	goose_destroy_logical_type(&value_type);
	goose_destroy_logical_type(&key_type);
}

TEST_CASE("Map value construction (invalid map type)", "[capi]") {
	auto int_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	goose::vector<goose_value> key_vals;
	goose::vector<goose_value> value_vals;
	auto map_value = goose_create_map_value(int_type, key_vals.data(), value_vals.data(), 0);
	REQUIRE(map_value == nullptr);
	goose_destroy_logical_type(&int_type);
}

TEST_CASE("Map value construction (invalid key array value types)", "[capi]") {
	auto key_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	auto value_type = goose_create_logical_type(GOOSE_TYPE_DOUBLE);
	auto map_type = goose_create_map_type(key_type, value_type);

	const char *keys[] {"a", "b", "c"}; // invalid type
	double values[] {1.2, 3.4, 5.6};
	idx_t entry_count = 3;

	goose::vector<goose_value> key_vals;
	goose::vector<goose_value> value_vals;
	for (idx_t i = 0; i < entry_count; ++i) {
		key_vals.push_back(goose_create_varchar(keys[i]));
		value_vals.push_back(goose_create_double(values[i]));
	}

	auto map_value = goose_create_map_value(map_type, key_vals.data(), value_vals.data(), entry_count);
	REQUIRE(map_value == nullptr);

	for (idx_t i = 0; i < entry_count; ++i) {
		goose_destroy_value(&key_vals[i]);
		goose_destroy_value(&value_vals[i]);
	}
	goose_destroy_logical_type(&map_type);
	goose_destroy_logical_type(&value_type);
	goose_destroy_logical_type(&key_type);
}

TEST_CASE("Map value construction (invalid value array value types)", "[capi]") {
	auto key_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	auto value_type = goose_create_logical_type(GOOSE_TYPE_DOUBLE);
	auto map_type = goose_create_map_type(key_type, value_type);

	int32_t keys[] {100, 200, 300};
	const char *values[] {"a", "b", "c"}; // invalid type
	idx_t entry_count = 3;

	goose::vector<goose_value> key_vals;
	goose::vector<goose_value> value_vals;
	for (idx_t i = 0; i < entry_count; ++i) {
		key_vals.push_back(goose_create_int32(keys[i]));
		value_vals.push_back(goose_create_varchar(values[i]));
	}

	auto map_value = goose_create_map_value(map_type, key_vals.data(), value_vals.data(), entry_count);
	REQUIRE(map_value == nullptr);

	for (idx_t i = 0; i < entry_count; ++i) {
		goose_destroy_value(&key_vals[i]);
		goose_destroy_value(&value_vals[i]);
	}
	goose_destroy_logical_type(&map_type);
	goose_destroy_logical_type(&value_type);
	goose_destroy_logical_type(&key_type);
}

TEST_CASE("Union value construction (happy path)", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	auto varchar_type = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
	auto int_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);

	goose::vector<goose_logical_type> member_types {varchar_type, int_type};
	goose::vector<const char *> member_names {"str", "int"};
	idx_t member_count = 2;

	auto union_type = goose_create_union_type(member_types.data(), member_names.data(), member_count);

	idx_t tag_index = 1;
	int32_t int32 = 42;
	auto int32_value = goose_create_int32(int32);

	auto union_value = goose_create_union_value(union_type, tag_index, int32_value);
	REQUIRE(union_value);

	goose_prepared_statement prepared;
	REQUIRE(goose_prepare(tester.connection, "select ?", &prepared) == GooseSuccess);

	goose_bind_value(prepared, 1, union_value);

	auto result = tester.QueryPrepared(prepared);
	REQUIRE(result->ChunkCount() == 1);
	auto capi_chunk = result->FetchChunk(0);
	REQUIRE(capi_chunk);
	auto chunk = capi_chunk->GetChunk();
	REQUIRE(chunk);
	auto row_count = goose_data_chunk_get_size(chunk);
	REQUIRE(row_count == 1);
	auto vector = goose_data_chunk_get_vector(chunk, 0);
	REQUIRE(vector);
	auto logical_type = goose_vector_get_column_type(vector);
	REQUIRE(logical_type);
	REQUIRE(goose_get_type_id(logical_type) == goose_type::GOOSE_TYPE_UNION);
	goose_destroy_logical_type(&logical_type);

	auto tags_vector = goose_struct_vector_get_child(vector, 0);
	REQUIRE(tags_vector);
	auto tags_data = (uint8_t *)goose_vector_get_data(tags_vector);
	REQUIRE(tags_data);
	REQUIRE(tags_data[0] == tag_index);
	auto value_vector = goose_struct_vector_get_child(vector, tag_index + 1);
	REQUIRE(value_vector);
	auto value_data = (int32_t *)goose_vector_get_data(value_vector);
	REQUIRE(value_data);
	REQUIRE(value_data[0] == int32);

	goose_destroy_prepare(&prepared);
	goose_destroy_value(&union_value);
	goose_destroy_value(&int32_value);
	goose_destroy_logical_type(&union_type);
	goose_destroy_logical_type(&int_type);
	goose_destroy_logical_type(&varchar_type);
}

TEST_CASE("Union value construction (null union type)", "[capi]") {
	auto value = goose_create_int32(42);
	auto union_value = goose_create_union_value(nullptr, 1, value);
	REQUIRE(union_value == nullptr);
	goose_destroy_value(&value);
}

TEST_CASE("Union value construction (null value pointer)", "[capi]") {
	auto varchar_type = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
	auto int_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	goose_logical_type member_types[] {varchar_type, int_type};
	const char *member_names[] {"str", "int"};
	idx_t member_count = 2;
	auto union_type = goose_create_union_type(member_types, member_names, member_count);
	auto union_value = goose_create_union_value(union_type, 1, nullptr);
	REQUIRE(union_value == nullptr);
	goose_destroy_logical_type(&union_type);
	goose_destroy_logical_type(&int_type);
	goose_destroy_logical_type(&varchar_type);
}

TEST_CASE("Union value construction (invalid union type)", "[capi]") {
	auto varchar_type = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
	auto value = goose_create_int32(42);
	auto union_value = goose_create_union_value(varchar_type, 1, value);
	REQUIRE(union_value == nullptr);
	goose_destroy_value(&value);
	goose_destroy_logical_type(&varchar_type);
}

TEST_CASE("Union value construction (invalid tag index)", "[capi]") {
	auto varchar_type = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
	auto int_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	goose_logical_type member_types[] {varchar_type, int_type};
	const char *member_names[] {"str", "int"};
	idx_t member_count = 2;
	auto union_type = goose_create_union_type(member_types, member_names, member_count);
	auto value = goose_create_int32(42);
	auto union_value = goose_create_union_value(union_type, 2, value);
	REQUIRE(union_value == nullptr);
	goose_destroy_value(&value);
	goose_destroy_logical_type(&union_type);
	goose_destroy_logical_type(&int_type);
	goose_destroy_logical_type(&varchar_type);
}

TEST_CASE("Union value construction (invalid value type)", "[capi]") {
	auto varchar_type = goose_create_logical_type(GOOSE_TYPE_VARCHAR);
	auto int_type = goose_create_logical_type(GOOSE_TYPE_INTEGER);
	goose_logical_type member_types[] {varchar_type, int_type};
	const char *member_names[] {"str", "int"};
	idx_t member_count = 2;
	auto union_type = goose_create_union_type(member_types, member_names, member_count);
	auto value = goose_create_double(1.2);
	auto union_value = goose_create_union_value(union_type, 1, value);
	REQUIRE(union_value == nullptr);
	goose_destroy_value(&value);
	goose_destroy_logical_type(&union_type);
	goose_destroy_logical_type(&int_type);
	goose_destroy_logical_type(&varchar_type);
}
