#include "capi_tester.h"

using namespace goose;
using namespace std;

TEST_CASE("Test logical type creation with unsupported types", "[capi]") {
	// Test goose_create_logical_type with unsupported types.
	goose::vector<goose_type> unsupported_types = {
	    GOOSE_TYPE_INVALID, GOOSE_TYPE_DECIMAL, GOOSE_TYPE_ENUM,  GOOSE_TYPE_LIST,
	    GOOSE_TYPE_STRUCT,  GOOSE_TYPE_MAP,     GOOSE_TYPE_ARRAY, GOOSE_TYPE_UNION,
	};

	for (const auto unsupported_type : unsupported_types) {
		auto logical_type = goose_create_logical_type(unsupported_type);
		REQUIRE(GOOSE_TYPE_INVALID == goose_get_type_id(logical_type));
		goose_destroy_logical_type(&logical_type);
	}
}

TEST_CASE("Test INVALID, ANY and SQLNULL", "[capi]") {
	auto sql_null_type = goose_create_logical_type(GOOSE_TYPE_SQLNULL);
	goose_destroy_logical_type(&sql_null_type);

	auto any_type = goose_create_logical_type(GOOSE_TYPE_ANY);
	auto invalid_type = goose_create_logical_type(GOOSE_TYPE_INVALID);

	auto result_type_id = goose_get_type_id(any_type);
	REQUIRE(result_type_id == GOOSE_TYPE_ANY);
	result_type_id = goose_get_type_id(invalid_type);
	REQUIRE(result_type_id == GOOSE_TYPE_INVALID);

	// LIST with ANY
	auto list = goose_create_list_type(any_type);
	result_type_id = goose_get_type_id(list);
	REQUIRE(result_type_id == GOOSE_TYPE_LIST);
	goose_destroy_logical_type(&list);

	// LIST with INVALID
	list = goose_create_list_type(invalid_type);
	result_type_id = goose_get_type_id(list);
	REQUIRE(result_type_id == GOOSE_TYPE_LIST);
	goose_destroy_logical_type(&list);

	// ARRAY with ANY
	auto array = goose_create_array_type(any_type, 2);
	result_type_id = goose_get_type_id(array);
	REQUIRE(result_type_id == GOOSE_TYPE_ARRAY);
	goose_destroy_logical_type(&array);

	// ARRAY with INVALID
	array = goose_create_array_type(invalid_type, 2);
	result_type_id = goose_get_type_id(array);
	REQUIRE(result_type_id == GOOSE_TYPE_ARRAY);
	goose_destroy_logical_type(&array);

	// MAP with ANY
	auto map = goose_create_map_type(any_type, any_type);
	result_type_id = goose_get_type_id(map);
	REQUIRE(result_type_id == GOOSE_TYPE_MAP);
	goose_destroy_logical_type(&map);

	// MAP with INVALID
	map = goose_create_map_type(any_type, any_type);
	result_type_id = goose_get_type_id(map);
	REQUIRE(result_type_id == GOOSE_TYPE_MAP);
	goose_destroy_logical_type(&map);

	// UNION with ANY and INVALID
	std::vector<const char *> member_names {"any", "invalid"};
	goose::vector<goose_logical_type> types = {any_type, invalid_type};
	auto union_type = goose_create_union_type(types.data(), member_names.data(), member_names.size());
	result_type_id = goose_get_type_id(union_type);
	REQUIRE(result_type_id == GOOSE_TYPE_UNION);
	goose_destroy_logical_type(&union_type);

	// Clean-up.
	goose_destroy_logical_type(&any_type);
	goose_destroy_logical_type(&invalid_type);
}

TEST_CASE("Test LIST and ARRAY with INVALID and ANY", "[capi]") {
	auto int_type = goose_create_logical_type(GOOSE_TYPE_BIGINT);
	auto any_type = goose_create_logical_type(GOOSE_TYPE_ANY);
	auto invalid_type = goose_create_logical_type(GOOSE_TYPE_INVALID);

	auto value = goose_create_int64(42);
	goose::vector<goose_value> list_values {value, value};

	auto int_list = goose_create_list_value(int_type, list_values.data(), list_values.size());
	auto result = goose_get_varchar(int_list);
	REQUIRE(string(result).compare("[42, 42]") == 0);
	goose_free(result);
	goose_destroy_value(&int_list);

	auto int_array = goose_create_array_value(int_type, list_values.data(), list_values.size());
	result = goose_get_varchar(int_array);
	REQUIRE(string(result).compare("[42, 42]") == 0);
	goose_free(result);
	goose_destroy_value(&int_array);

	auto invalid_list = goose_create_list_value(any_type, list_values.data(), list_values.size());
	REQUIRE(invalid_list == nullptr);
	auto invalid_array = goose_create_array_value(any_type, list_values.data(), list_values.size());
	REQUIRE(invalid_array == nullptr);
	auto any_list = goose_create_list_value(any_type, list_values.data(), list_values.size());
	REQUIRE(any_list == nullptr);
	auto any_array = goose_create_array_value(any_type, list_values.data(), list_values.size());
	REQUIRE(any_array == nullptr);

	// Clean-up.
	goose_destroy_value(&value);
	goose_destroy_logical_type(&int_type);
	goose_destroy_logical_type(&any_type);
	goose_destroy_logical_type(&invalid_type);
}

TEST_CASE("Test STRUCT with INVALID and ANY", "[capi]") {
	auto int_type = goose_create_logical_type(GOOSE_TYPE_BIGINT);
	auto any_type = goose_create_logical_type(GOOSE_TYPE_ANY);
	auto invalid_type = goose_create_logical_type(GOOSE_TYPE_INVALID);

	auto value = goose_create_int64(42);
	goose::vector<goose_value> struct_values {value, value};

	// Test goose_create_struct_type with ANY.
	std::vector<const char *> member_names {"int", "other"};
	goose::vector<goose_logical_type> types = {int_type, any_type};
	auto struct_type = goose_create_struct_type(types.data(), member_names.data(), member_names.size());
	REQUIRE(struct_type != nullptr);

	// Test goose_create_struct_value with ANY.
	auto struct_value = goose_create_struct_value(struct_type, struct_values.data());
	REQUIRE(struct_value == nullptr);
	goose_destroy_logical_type(&struct_type);

	// Test goose_create_struct_type with INVALID.
	types = {int_type, invalid_type};
	struct_type = goose_create_struct_type(types.data(), member_names.data(), member_names.size());
	REQUIRE(struct_type != nullptr);

	// Test goose_create_struct_value with INVALID.
	struct_value = goose_create_struct_value(struct_type, struct_values.data());
	REQUIRE(struct_value == nullptr);
	goose_destroy_logical_type(&struct_type);

	// Clean-up.
	goose_destroy_value(&value);
	goose_destroy_logical_type(&int_type);
	goose_destroy_logical_type(&any_type);
	goose_destroy_logical_type(&invalid_type);
}

TEST_CASE("Test data chunk creation with INVALID and ANY types", "[capi]") {
	auto any_type = goose_create_logical_type(GOOSE_TYPE_ANY);
	auto invalid_type = goose_create_logical_type(GOOSE_TYPE_INVALID);
	auto list_type = goose_create_list_type(any_type);

	// For each type, try to create a data chunk with that type.
	std::vector<goose_logical_type> test_types = {any_type, invalid_type, list_type};
	for (idx_t i = 0; i < test_types.size(); i++) {
		goose_logical_type types[1];
		types[0] = test_types[i];
		auto data_chunk = goose_create_data_chunk(types, 1);
		REQUIRE(data_chunk == nullptr);
	}

	// Clean-up.
	goose_destroy_logical_type(&list_type);
	goose_destroy_logical_type(&any_type);
	goose_destroy_logical_type(&invalid_type);
}

void DummyScalar(goose_function_info, goose_data_chunk, goose_vector) {
}

static goose_scalar_function DummyScalarFunction() {
	auto function = goose_create_scalar_function();
	goose_scalar_function_set_name(function, "hello");
	goose_scalar_function_set_function(function, DummyScalar);
	return function;
}

static void TestScalarFunction(goose_scalar_function function, goose_connection connection) {
	auto status = goose_register_scalar_function(connection, function);
	REQUIRE(status == GooseError);
	goose_destroy_scalar_function(&function);
}

TEST_CASE("Test scalar functions with INVALID and ANY types", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	REQUIRE(tester.OpenDatabase(nullptr));

	auto int_type = goose_create_logical_type(GOOSE_TYPE_BIGINT);
	auto any_type = goose_create_logical_type(GOOSE_TYPE_ANY);
	auto invalid_type = goose_create_logical_type(GOOSE_TYPE_INVALID);

	// Set INVALID as a parameter.
	auto function = DummyScalarFunction();
	goose_scalar_function_add_parameter(function, invalid_type);
	goose_scalar_function_set_return_type(function, int_type);
	TestScalarFunction(function, tester.connection);

	// Set INVALID as the return type.
	function = DummyScalarFunction();
	goose_scalar_function_set_return_type(function, invalid_type);
	TestScalarFunction(function, tester.connection);

	// Set ANY as the return type.
	function = DummyScalarFunction();
	goose_scalar_function_set_return_type(function, any_type);
	TestScalarFunction(function, tester.connection);

	// Clean-up.
	goose_destroy_logical_type(&int_type);
	goose_destroy_logical_type(&any_type);
	goose_destroy_logical_type(&invalid_type);
}

void my_dummy_bind(goose_bind_info) {
}

void my_dummy_init(goose_init_info) {
}

void my_dummy_function(goose_function_info, goose_data_chunk) {
}

static goose_table_function DummyTableFunction() {
	auto function = goose_create_table_function();
	goose_table_function_set_name(function, "hello");
	goose_table_function_set_bind(function, my_dummy_bind);
	goose_table_function_set_init(function, my_dummy_init);
	goose_table_function_set_function(function, my_dummy_function);
	return function;
}

static void TestTableFunction(goose_table_function function, goose_connection connection) {
	auto status = goose_register_table_function(connection, function);
	REQUIRE(status == GooseError);
	goose_destroy_table_function(&function);
}

TEST_CASE("Test table functions with INVALID and ANY types", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;
	REQUIRE(tester.OpenDatabase(nullptr));

	auto invalid_type = goose_create_logical_type(GOOSE_TYPE_INVALID);

	// Set INVALID as a parameter.
	auto function = DummyTableFunction();
	goose_table_function_add_parameter(function, invalid_type);
	TestTableFunction(function, tester.connection);

	// Set INVALID as a named parameter.
	function = DummyTableFunction();
	goose_table_function_add_named_parameter(function, "my_parameter", invalid_type);
	TestTableFunction(function, tester.connection);

	goose_destroy_logical_type(&invalid_type);
}
