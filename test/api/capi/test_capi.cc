#include "capi_tester.h"
#include <regex>

using namespace goose;
using namespace std;

static void require_hugeint_eq(goose_hugeint left, goose_hugeint right) {
	REQUIRE(left.lower == right.lower);
	REQUIRE(left.upper == right.upper);
}

static void require_hugeint_eq(goose_hugeint left, uint64_t lower, int64_t upper) {
	goose_hugeint temp;
	temp.lower = lower;
	temp.upper = upper;
	require_hugeint_eq(left, temp);
}

static void require_uhugeint_eq(goose_uhugeint left, goose_uhugeint right) {
	REQUIRE(left.lower == right.lower);
	REQUIRE(left.upper == right.upper);
}

static void require_uhugeint_eq(goose_uhugeint left, uint64_t lower, uint64_t upper) {
	goose_uhugeint temp;
	temp.lower = lower;
	temp.upper = upper;
	require_uhugeint_eq(left, temp);
}

TEST_CASE("Basic test of C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	// open the database in in-memory mode
	REQUIRE(tester.OpenDatabase(nullptr));

	REQUIRE_NO_FAIL(tester.Query("SET default_null_order='nulls_first'"));
	// select scalar value
	result = tester.Query("SELECT CAST(42 AS BIGINT)");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->ColumnType(0) == GOOSE_TYPE_BIGINT);
	REQUIRE(result->ColumnData<int64_t>(0)[0] == 42);
	REQUIRE(result->ColumnCount() == 1);
	REQUIRE(result->row_count() == 1);
	REQUIRE(result->Fetch<int64_t>(0, 0) == 42);
	REQUIRE(!result->IsNull(0, 0));
	// out of range fetch
	REQUIRE(result->Fetch<int64_t>(1, 0) == 0);
	REQUIRE(result->Fetch<int64_t>(0, 1) == 0);
	// cannot fetch data chunk after using the value API
	REQUIRE(result->FetchChunk(0) == nullptr);

	// select scalar NULL
	result = tester.Query("SELECT NULL");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->ColumnCount() == 1);
	REQUIRE(result->row_count() == 1);
	REQUIRE(result->Fetch<int64_t>(0, 0) == 0);
	REQUIRE(result->IsNull(0, 0));

	// select scalar string
	result = tester.Query("SELECT 'hello'");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->ColumnCount() == 1);
	REQUIRE(result->row_count() == 1);
	REQUIRE(result->Fetch<string>(0, 0) == "hello");
	REQUIRE(!result->IsNull(0, 0));

	result = tester.Query("SELECT 1=1");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->ColumnCount() == 1);
	REQUIRE(result->row_count() == 1);
	REQUIRE(result->Fetch<bool>(0, 0) == true);
	REQUIRE(!result->IsNull(0, 0));

	result = tester.Query("SELECT 1=0");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->ColumnCount() == 1);
	REQUIRE(result->row_count() == 1);
	REQUIRE(result->Fetch<bool>(0, 0) == false);
	REQUIRE(!result->IsNull(0, 0));

	result = tester.Query("SELECT i FROM (values (true), (false)) tbl(i) group by i order by i");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->ColumnCount() == 1);
	REQUIRE(result->row_count() == 2);
	REQUIRE(result->Fetch<bool>(0, 0) == false);
	REQUIRE(result->Fetch<bool>(0, 1) == true);
	REQUIRE(!result->IsNull(0, 0));

	// multiple insertions
	REQUIRE_NO_FAIL(tester.Query("CREATE TABLE test (a INTEGER, b INTEGER);"));
	REQUIRE_NO_FAIL(tester.Query("INSERT INTO test VALUES (11, 22)"));
	REQUIRE_NO_FAIL(tester.Query("INSERT INTO test VALUES (NULL, 21)"));
	result = tester.Query("INSERT INTO test VALUES (13, 22)");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->rows_changed() == 1);

	// NULL selection
	result = tester.Query("SELECT a, b FROM test ORDER BY a");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->rows_changed() == 0);
	// NULL, 11, 13
	REQUIRE(result->IsNull(0, 0));
	REQUIRE(result->Fetch<int32_t>(0, 1) == 11);
	REQUIRE(result->Fetch<int32_t>(0, 2) == 13);
	// 21, 22, 22
	REQUIRE(result->Fetch<int32_t>(1, 0) == 21);
	REQUIRE(result->Fetch<int32_t>(1, 1) == 22);
	REQUIRE(result->Fetch<int32_t>(1, 2) == 22);

	REQUIRE(result->ColumnName(0) == "a");
	REQUIRE(result->ColumnName(1) == "b");
	REQUIRE(result->ColumnName(2) == "");

	result = tester.Query("UPDATE test SET a = 1 WHERE b=22");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->rows_changed() == 2);

	// several error conditions
	REQUIRE(goose_value_is_null(nullptr, 0, 0) == false);
	REQUIRE(goose_column_type(nullptr, 0) == GOOSE_TYPE_INVALID);
	REQUIRE(goose_column_count(nullptr) == 0);
	REQUIRE(goose_row_count(nullptr) == 0);
	REQUIRE(goose_rows_changed(nullptr) == 0);
	REQUIRE(goose_result_error(nullptr) == nullptr);
	REQUIRE(goose_nullmask_data(nullptr, 0) == nullptr);
	REQUIRE(goose_column_data(nullptr, 0) == nullptr);
	REQUIRE(goose_result_error_type(nullptr) == GOOSE_ERROR_INVALID);
}

TEST_CASE("Test different types of C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	// open the database in in-memory mode
	REQUIRE(tester.OpenDatabase(nullptr));
	REQUIRE_NO_FAIL(tester.Query("SET default_null_order='nulls_first'"));

	// integer columns
	goose::vector<string> types = {"TINYINT",  "SMALLINT",  "INTEGER",  "BIGINT",  "HUGEINT",
	                                "UTINYINT", "USMALLINT", "UINTEGER", "UBIGINT", "UHUGEINT"};
	for (auto &type : types) {
		// create the table and insert values
		REQUIRE_NO_FAIL(tester.Query("BEGIN TRANSACTION"));
		REQUIRE_NO_FAIL(tester.Query("CREATE TABLE integers(i " + type + ")"));
		REQUIRE_NO_FAIL(tester.Query("INSERT INTO integers VALUES (1), (NULL)"));

		result = tester.Query("SELECT * FROM integers ORDER BY i");
		REQUIRE_NO_FAIL(*result);
		REQUIRE(result->IsNull(0, 0));
		REQUIRE(result->Fetch<int8_t>(0, 0) == 0);
		REQUIRE(result->Fetch<int16_t>(0, 0) == 0);
		REQUIRE(result->Fetch<int32_t>(0, 0) == 0);
		REQUIRE(result->Fetch<int64_t>(0, 0) == 0);
		REQUIRE(result->Fetch<uint8_t>(0, 0) == 0);
		REQUIRE(result->Fetch<uint16_t>(0, 0) == 0);
		REQUIRE(result->Fetch<uint32_t>(0, 0) == 0);
		REQUIRE(result->Fetch<uint64_t>(0, 0) == 0);
		REQUIRE(goose_uhugeint_to_double(result->Fetch<goose_uhugeint>(0, 0)) == 0);
		REQUIRE(goose_hugeint_to_double(result->Fetch<goose_hugeint>(0, 0)) == 0);
		REQUIRE(result->Fetch<string>(0, 0) == "");
		REQUIRE(ApproxEqual(result->Fetch<float>(0, 0), 0.0f));
		REQUIRE(ApproxEqual(result->Fetch<double>(0, 0), 0.0));

		REQUIRE(!result->IsNull(0, 1));
		REQUIRE(result->Fetch<int8_t>(0, 1) == 1);
		REQUIRE(result->Fetch<int16_t>(0, 1) == 1);
		REQUIRE(result->Fetch<int32_t>(0, 1) == 1);
		REQUIRE(result->Fetch<int64_t>(0, 1) == 1);
		REQUIRE(result->Fetch<uint8_t>(0, 1) == 1);
		REQUIRE(result->Fetch<uint16_t>(0, 1) == 1);
		REQUIRE(result->Fetch<uint32_t>(0, 1) == 1);
		REQUIRE(result->Fetch<uint64_t>(0, 1) == 1);
		REQUIRE(goose_uhugeint_to_double(result->Fetch<goose_uhugeint>(0, 1)) == 1);
		REQUIRE(goose_hugeint_to_double(result->Fetch<goose_hugeint>(0, 1)) == 1);
		REQUIRE(ApproxEqual(result->Fetch<float>(0, 1), 1.0f));
		REQUIRE(ApproxEqual(result->Fetch<double>(0, 1), 1.0));
		REQUIRE(result->Fetch<string>(0, 1) == "1");

		REQUIRE_NO_FAIL(tester.Query("ROLLBACK"));
	}
	// real/double columns
	types = {"REAL", "DOUBLE"};
	for (auto &type : types) {
		// create the table and insert values
		REQUIRE_NO_FAIL(tester.Query("BEGIN TRANSACTION"));
		REQUIRE_NO_FAIL(tester.Query("CREATE TABLE doubles(i " + type + ")"));
		REQUIRE_NO_FAIL(tester.Query("INSERT INTO doubles VALUES (1), (NULL)"));

		result = tester.Query("SELECT * FROM doubles ORDER BY i");
		REQUIRE_NO_FAIL(*result);
		REQUIRE(result->IsNull(0, 0));
		REQUIRE(result->Fetch<int8_t>(0, 0) == 0);
		REQUIRE(result->Fetch<int16_t>(0, 0) == 0);
		REQUIRE(result->Fetch<int32_t>(0, 0) == 0);
		REQUIRE(result->Fetch<int64_t>(0, 0) == 0);
		REQUIRE(result->Fetch<string>(0, 0) == "");
		REQUIRE(ApproxEqual(result->Fetch<float>(0, 0), 0.0f));
		REQUIRE(ApproxEqual(result->Fetch<double>(0, 0), 0.0));

		REQUIRE(!result->IsNull(0, 1));
		REQUIRE(result->Fetch<int8_t>(0, 1) == 1);
		REQUIRE(result->Fetch<int16_t>(0, 1) == 1);
		REQUIRE(result->Fetch<int32_t>(0, 1) == 1);
		REQUIRE(result->Fetch<int64_t>(0, 1) == 1);
		REQUIRE(ApproxEqual(result->Fetch<float>(0, 1), 1.0f));
		REQUIRE(ApproxEqual(result->Fetch<double>(0, 1), 1.0));

		REQUIRE_NO_FAIL(tester.Query("ROLLBACK"));
	}
	// date columns
	REQUIRE_NO_FAIL(tester.Query("CREATE TABLE dates(d DATE)"));
	REQUIRE_NO_FAIL(tester.Query("INSERT INTO dates VALUES ('1992-09-20'), (NULL), ('30000-09-20')"));

	result = tester.Query("SELECT * FROM dates ORDER BY d");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->IsNull(0, 0));
	goose_date_struct date = goose_from_date(result->Fetch<goose_date>(0, 1));
	REQUIRE(date.year == 1992);
	REQUIRE(date.month == 9);
	REQUIRE(date.day == 20);
	REQUIRE(result->Fetch<string>(0, 1) == Value::DATE(1992, 9, 20).ToString());
	date = goose_from_date(result->Fetch<goose_date>(0, 2));
	REQUIRE(date.year == 30000);
	REQUIRE(date.month == 9);
	REQUIRE(date.day == 20);
	REQUIRE(result->Fetch<string>(0, 2) == Value::DATE(30000, 9, 20).ToString());

	// time columns
	REQUIRE_NO_FAIL(tester.Query("CREATE TABLE times(d TIME)"));
	REQUIRE_NO_FAIL(tester.Query("INSERT INTO times VALUES ('12:00:30.1234'), (NULL), ('02:30:01')"));

	result = tester.Query("SELECT * FROM times ORDER BY d");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->IsNull(0, 0));
	goose_time_struct time_val = goose_from_time(result->Fetch<goose_time>(0, 1));
	REQUIRE(time_val.hour == 2);
	REQUIRE(time_val.min == 30);
	REQUIRE(time_val.sec == 1);
	REQUIRE(time_val.micros == 0);
	REQUIRE(result->Fetch<string>(0, 1) == Value::TIME(2, 30, 1, 0).ToString());
	time_val = goose_from_time(result->Fetch<goose_time>(0, 2));
	REQUIRE(time_val.hour == 12);
	REQUIRE(time_val.min == 0);
	REQUIRE(time_val.sec == 30);
	REQUIRE(time_val.micros == 123400);
	REQUIRE(result->Fetch<string>(0, 2) == Value::TIME(12, 0, 30, 123400).ToString());

	// blob columns
	REQUIRE_NO_FAIL(tester.Query("CREATE TABLE blobs(b BLOB)"));
	REQUIRE_NO_FAIL(tester.Query("INSERT INTO blobs VALUES ('hello\\x12world'), ('\\x00'), (NULL)"));

	result = tester.Query("SELECT * FROM blobs");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(!result->IsNull(0, 0));
	goose_blob blob = result->Fetch<goose_blob>(0, 0);
	REQUIRE(blob.size == 11);
	REQUIRE(memcmp(blob.data, "hello\012world", 11));
	REQUIRE(result->Fetch<string>(0, 1) == "\\x00");
	REQUIRE(result->IsNull(0, 2));
	blob = result->Fetch<goose_blob>(0, 2);
	REQUIRE(blob.data == nullptr);
	REQUIRE(blob.size == 0);

	// boolean columns
	REQUIRE_NO_FAIL(tester.Query("CREATE TABLE booleans(b BOOLEAN)"));
	REQUIRE_NO_FAIL(tester.Query("INSERT INTO booleans VALUES (42 > 60), (42 > 20), (42 > NULL)"));

	result = tester.Query("SELECT * FROM booleans ORDER BY b");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->IsNull(0, 0));
	REQUIRE(!result->Fetch<bool>(0, 0));
	REQUIRE(!result->Fetch<bool>(0, 1));
	REQUIRE(result->Fetch<bool>(0, 2));
	REQUIRE(result->Fetch<string>(0, 2) == "true");

	// decimal columns
	REQUIRE_NO_FAIL(tester.Query("CREATE TABLE decimals(dec DECIMAL(18, 4) NULL)"));
	REQUIRE_NO_FAIL(tester.Query("INSERT INTO decimals VALUES (NULL), (12.3)"));

	result = tester.Query("SELECT * FROM decimals ORDER BY dec");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->IsNull(0, 0));
	goose_decimal decimal = result->Fetch<goose_decimal>(0, 1);
	REQUIRE(goose_decimal_to_double(decimal) == 12.3);
	// test more decimal physical types
	result = tester.Query("SELECT "
	                      "1.2::DECIMAL(4,1),"
	                      "100.3::DECIMAL(9,1),"
	                      "-320938.4298::DECIMAL(18,4),"
	                      "49082094824.904820482094::DECIMAL(30,12),"
	                      "NULL::DECIMAL");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(goose_decimal_to_double(result->Fetch<goose_decimal>(0, 0)) == 1.2);
	REQUIRE(goose_decimal_to_double(result->Fetch<goose_decimal>(1, 0)) == 100.3);
	REQUIRE(goose_decimal_to_double(result->Fetch<goose_decimal>(2, 0)) == -320938.4298);
	REQUIRE(goose_decimal_to_double(result->Fetch<goose_decimal>(3, 0)) == 49082094824.904820482094);
	REQUIRE(goose_decimal_to_double(result->Fetch<goose_decimal>(4, 0)) == 0.0);

	REQUIRE(!result->IsNull(0, 0));
	REQUIRE(!result->IsNull(1, 0));
	REQUIRE(!result->IsNull(2, 0));
	REQUIRE(!result->IsNull(3, 0));
	REQUIRE(result->IsNull(4, 0));

	REQUIRE(result->Fetch<bool>(0, 0) == true);
	REQUIRE(result->Fetch<bool>(1, 0) == true);
	REQUIRE(result->Fetch<bool>(2, 0) == true);
	REQUIRE(result->Fetch<bool>(3, 0) == true);
	REQUIRE(result->Fetch<bool>(4, 0) == false);

	REQUIRE(result->Fetch<int8_t>(0, 0) == 1);
	REQUIRE(result->Fetch<int8_t>(1, 0) == 100);
	REQUIRE(result->Fetch<int8_t>(2, 0) == 0); // overflow
	REQUIRE(result->Fetch<int8_t>(3, 0) == 0); // overflow
	REQUIRE(result->Fetch<int8_t>(4, 0) == 0);

	REQUIRE(result->Fetch<uint8_t>(0, 0) == 1);
	REQUIRE(result->Fetch<uint8_t>(1, 0) == 100);
	REQUIRE(result->Fetch<uint8_t>(2, 0) == 0); // overflow
	REQUIRE(result->Fetch<uint8_t>(3, 0) == 0); // overflow
	REQUIRE(result->Fetch<uint8_t>(4, 0) == 0);

	REQUIRE(result->Fetch<int16_t>(0, 0) == 1);
	REQUIRE(result->Fetch<int16_t>(1, 0) == 100);
	REQUIRE(result->Fetch<int16_t>(2, 0) == 0); // overflow
	REQUIRE(result->Fetch<int16_t>(3, 0) == 0); // overflow
	REQUIRE(result->Fetch<int16_t>(4, 0) == 0);

	REQUIRE(result->Fetch<uint16_t>(0, 0) == 1);
	REQUIRE(result->Fetch<uint16_t>(1, 0) == 100);
	REQUIRE(result->Fetch<uint16_t>(2, 0) == 0); // overflow
	REQUIRE(result->Fetch<uint16_t>(3, 0) == 0); // overflow
	REQUIRE(result->Fetch<uint16_t>(4, 0) == 0);

	REQUIRE(result->Fetch<int32_t>(0, 0) == 1);
	REQUIRE(result->Fetch<int32_t>(1, 0) == 100);
	REQUIRE(result->Fetch<int32_t>(2, 0) == -320938);
	REQUIRE(result->Fetch<int32_t>(3, 0) == 0); // overflow
	REQUIRE(result->Fetch<int32_t>(4, 0) == 0);

	REQUIRE(result->Fetch<uint32_t>(0, 0) == 1);
	REQUIRE(result->Fetch<uint32_t>(1, 0) == 100);
	REQUIRE(result->Fetch<uint32_t>(2, 0) == 0); // overflow
	REQUIRE(result->Fetch<uint32_t>(3, 0) == 0); // overflow
	REQUIRE(result->Fetch<uint32_t>(4, 0) == 0);

	REQUIRE(result->Fetch<int64_t>(0, 0) == 1);
	REQUIRE(result->Fetch<int64_t>(1, 0) == 100);
	REQUIRE(result->Fetch<int64_t>(2, 0) == -320938);
	REQUIRE(result->Fetch<int64_t>(3, 0) == 49082094825); // ceiling
	REQUIRE(result->Fetch<int64_t>(4, 0) == 0);

	REQUIRE(result->Fetch<uint64_t>(0, 0) == 1);
	REQUIRE(result->Fetch<uint64_t>(1, 0) == 100);
	REQUIRE(result->Fetch<uint64_t>(2, 0) == 0); // overflow
	REQUIRE(result->Fetch<uint64_t>(3, 0) == 49082094825);
	REQUIRE(result->Fetch<uint64_t>(4, 0) == 0);

	require_hugeint_eq(result->Fetch<goose_hugeint>(0, 0), 1, 0);
	require_hugeint_eq(result->Fetch<goose_hugeint>(1, 0), 100, 0);
	require_hugeint_eq(result->Fetch<goose_hugeint>(2, 0), 18446744073709230678ul, -1);
	require_hugeint_eq(result->Fetch<goose_hugeint>(3, 0), 49082094825, 0);
	require_hugeint_eq(result->Fetch<goose_hugeint>(4, 0), 0, 0);

	require_uhugeint_eq(result->Fetch<goose_uhugeint>(0, 0), 1, 0);
	require_uhugeint_eq(result->Fetch<goose_uhugeint>(1, 0), 100, 0);
	require_uhugeint_eq(result->Fetch<goose_uhugeint>(2, 0), 0, 0); // overflow
	require_uhugeint_eq(result->Fetch<goose_uhugeint>(3, 0), 49082094825, 0);
	require_uhugeint_eq(result->Fetch<goose_uhugeint>(4, 0), 0, 0);

	REQUIRE(result->Fetch<float>(0, 0) == 1.2f);
	REQUIRE(result->Fetch<float>(1, 0) == 100.3f);
	REQUIRE(floor(result->Fetch<float>(2, 0)) == -320939);
	REQUIRE((int64_t)floor(result->Fetch<float>(3, 0)) == 49082093568);
	REQUIRE(result->Fetch<float>(4, 0) == 0.0);

	REQUIRE(result->Fetch<double>(0, 0) == 1.2);
	REQUIRE(result->Fetch<double>(1, 0) == 100.3);
	REQUIRE(result->Fetch<double>(2, 0) == -320938.4298);
	REQUIRE(result->Fetch<double>(3, 0) == 49082094824.904820482094);
	REQUIRE(result->Fetch<double>(4, 0) == 0.0);

	REQUIRE(result->Fetch<string>(0, 0) == "1.2");
	REQUIRE(result->Fetch<string>(1, 0) == "100.3");
	REQUIRE(result->Fetch<string>(2, 0) == "-320938.4298");
	REQUIRE(result->Fetch<string>(3, 0) == "49082094824.904820482094");
	REQUIRE(result->Fetch<string>(4, 0) == "");

	result = tester.Query("SELECT -123.45::DECIMAL(5,2)");
	REQUIRE_NO_FAIL(*result);
	REQUIRE(result->Fetch<bool>(0, 0) == true);
	REQUIRE(result->Fetch<int8_t>(0, 0) == -123);
	REQUIRE(result->Fetch<uint8_t>(0, 0) == 0);
	REQUIRE(result->Fetch<int16_t>(0, 0) == -123);
	REQUIRE(result->Fetch<uint16_t>(0, 0) == 0);
	REQUIRE(result->Fetch<int32_t>(0, 0) == -123);
	REQUIRE(result->Fetch<uint32_t>(0, 0) == 0);
	REQUIRE(result->Fetch<int64_t>(0, 0) == -123);
	REQUIRE(result->Fetch<uint64_t>(0, 0) == 0);

	hugeint_t expected_hugeint_val;
	Hugeint::TryConvert(-123, expected_hugeint_val);
	goose_hugeint expected_val;
	expected_val.lower = expected_hugeint_val.lower;
	expected_val.upper = expected_hugeint_val.upper;
	require_hugeint_eq(result->Fetch<goose_hugeint>(0, 0), expected_val);

	REQUIRE(result->Fetch<float>(0, 0) == -123.45f);
	REQUIRE(result->Fetch<double>(0, 0) == -123.45);
	REQUIRE(result->Fetch<string>(0, 0) == "-123.45");
}

TEST_CASE("decompose timetz with goose_from_time_tz", "[capi]") {
	CAPITester tester;

	REQUIRE(tester.OpenDatabase(nullptr));

	auto res = tester.Query("SELECT TIMETZ '11:30:00.123456-02:00'");
	REQUIRE(res->success);

	auto chunk = res->FetchChunk(0);

	REQUIRE(chunk->ColumnCount() == 1);
	REQUIRE(res->ColumnType(0) == GOOSE_TYPE_TIME_TZ);

	auto data = (goose_time_tz *)chunk->GetData(0);

	auto time_tz = goose_from_time_tz(data[0]);

	REQUIRE(time_tz.time.hour == 11);
	REQUIRE(time_tz.time.min == 30);
	REQUIRE(time_tz.time.sec == 0);
	REQUIRE(time_tz.time.micros == 123456);

	REQUIRE(time_tz.offset == -7200);
}

TEST_CASE("create time_tz value") {
	goose_time_struct time;
	time.hour = 4;
	time.min = 2;
	time.sec = 6;
	time.micros = 9;
	int offset = 8000;

	auto micros = goose_to_time(time);
	auto res = goose_create_time_tz(micros.micros, offset);

	// and back again

	auto inverse = goose_from_time_tz(res);
	REQUIRE(offset == inverse.offset);

	REQUIRE(inverse.time.hour == 4);
	REQUIRE(inverse.time.min == 2);
	REQUIRE(inverse.time.sec == 6);
	REQUIRE(inverse.time.micros == 9);
}

TEST_CASE("Test errors in C API", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	// cannot open database in random directory
	REQUIRE(!tester.OpenDatabase("/bla/this/directory/should/not/exist/hopefully/awerar333"));
	REQUIRE(tester.OpenDatabase(nullptr));

	// syntax error in query
	REQUIRE_FAIL(tester.Query("SELEC * FROM TABLE"));
	// bind error
	REQUIRE_FAIL(tester.Query("SELECT * FROM TABLE"));

	goose_result res;
	goose_prepared_statement stmt = nullptr;
	// fail prepare API calls
	REQUIRE(goose_prepare(NULL, "SELECT 42", &stmt) == GooseError);
	REQUIRE(goose_prepare(tester.connection, NULL, &stmt) == GooseError);
	REQUIRE(stmt == nullptr);

	REQUIRE(goose_prepare(tester.connection, "SELECT * from INVALID_TABLE", &stmt) == GooseError);
	REQUIRE(goose_prepare_error(nullptr) == nullptr);
	REQUIRE(stmt != nullptr);
	REQUIRE(goose_prepare_error(stmt) != nullptr);
	goose_destroy_prepare(&stmt);

	REQUIRE(goose_bind_boolean(NULL, 0, true) == GooseError);
	REQUIRE(goose_execute_prepared(NULL, &res) == GooseError);
	goose_destroy_prepare(NULL);

	// fail to query arrow
	goose_arrow out_arrow;
	REQUIRE(goose_query_arrow(tester.connection, "SELECT * from INVALID_TABLE", &out_arrow) == GooseError);
	REQUIRE(goose_query_arrow_error(out_arrow) != nullptr);
	goose_destroy_arrow(&out_arrow);

	// various edge cases/nullptrs
	REQUIRE(goose_query_arrow_schema(out_arrow, nullptr) == GooseSuccess);
	REQUIRE(goose_query_arrow_array(out_arrow, nullptr) == GooseSuccess);

	// default goose_value_date on invalid date
	result = tester.Query("SELECT 1, true, 'a'");
	REQUIRE_NO_FAIL(*result);
	goose_date_struct d = result->Fetch<goose_date_struct>(0, 0);
	REQUIRE(d.year == 1970);
	REQUIRE(d.month == 1);
	REQUIRE(d.day == 1);
	d = result->Fetch<goose_date_struct>(1, 0);
	REQUIRE(d.year == 1970);
	REQUIRE(d.month == 1);
	REQUIRE(d.day == 1);
	d = result->Fetch<goose_date_struct>(2, 0);
	REQUIRE(d.year == 1970);
	REQUIRE(d.month == 1);
	REQUIRE(d.day == 1);
}

TEST_CASE("Test C API config", "[capi]") {
	goose_database db = nullptr;
	goose_connection con = nullptr;
	goose_config config = nullptr;
	goose_result result;

	// enumerate config options
	auto config_count = goose_config_count();
	for (size_t i = 0; i < config_count; i++) {
		const char *name = nullptr;
		const char *description = nullptr;
		goose_get_config_flag(i, &name, &description);
		REQUIRE(strlen(name) > 0);
		REQUIRE(strlen(description) > 0);
	}

	// test config creation
	REQUIRE(goose_create_config(&config) == GooseSuccess);
	REQUIRE(goose_set_config(config, "access_mode", "invalid_access_mode") == GooseError);
	REQUIRE(goose_set_config(config, "access_mode", "read_only") == GooseSuccess);

	auto dbdir = TestCreatePath("capi_read_only_db");

	// open the database & connection
	// cannot open an in-memory database in read-only mode
	char *error = nullptr;
	REQUIRE(goose_open_ext(":memory:", &db, config, &error) == GooseError);
	REQUIRE(strlen(error) > 0);
	goose_free(error);
	// now without the error
	REQUIRE(goose_open_ext(":memory:", &db, config, nullptr) == GooseError);
	// cannot open a database that does not exist
	REQUIRE(goose_open_ext(dbdir.c_str(), &db, config, &error) == GooseError);
	REQUIRE(strlen(error) > 0);
	goose_free(error);
	// we can create the database and add some tables
	{
		Goose cppdb(dbdir);
		Connection cppcon(cppdb);
		cppcon.Query("CREATE TABLE integers(i INTEGER)");
		cppcon.Query("INSERT INTO integers VALUES (42)");
	}

	// now we can connect
	REQUIRE(goose_open_ext(dbdir.c_str(), &db, config, &error) == GooseSuccess);

	// test unrecognized configuration
	REQUIRE(goose_set_config(config, "aaaa_invalidoption", "read_only") == GooseSuccess);
	REQUIRE(((DBConfig *)config)->options.unrecognized_options["aaaa_invalidoption"] == "read_only");
	REQUIRE(goose_open_ext(dbdir.c_str(), &db, config, &error) == GooseError);
	REQUIRE_THAT(error, Catch::Matchers::Contains("The following options were not recognized"));
	goose_free(error);

	// we can destroy the config right after goose_open
	goose_destroy_config(&config);
	// we can spam this
	goose_destroy_config(&config);
	goose_destroy_config(&config);

	REQUIRE(goose_connect(db, nullptr) == GooseError);
	REQUIRE(goose_connect(nullptr, &con) == GooseError);
	REQUIRE(goose_connect(db, &con) == GooseSuccess);

	// we can query
	REQUIRE(goose_query(con, "SELECT 42::INT", &result) == GooseSuccess);
	REQUIRE(goose_value_int32(&result, 0, 0) == 42);
	goose_destroy_result(&result);
	REQUIRE(goose_query(con, "SELECT i::INT FROM integers", &result) == GooseSuccess);
	REQUIRE(goose_value_int32(&result, 0, 0) == 42);
	goose_destroy_result(&result);

	// but we cannot create new tables
	REQUIRE(goose_query(con, "CREATE TABLE new_table(i INTEGER)", nullptr) == GooseError);

	goose_disconnect(&con);
	goose_close(&db);

	// api abuse
	REQUIRE(goose_create_config(nullptr) == GooseError);
	REQUIRE(goose_get_config_flag(9999999, nullptr, nullptr) == GooseError);
	REQUIRE(goose_set_config(nullptr, nullptr, nullptr) == GooseError);
	REQUIRE(goose_create_config(nullptr) == GooseError);
	goose_destroy_config(nullptr);
	goose_destroy_config(nullptr);
}

TEST_CASE("Issue #2058: Cleanup after execution of invalid SQL statement causes segmentation fault", "[capi]") {
	goose_database db;
	goose_connection con;
	goose_result result;
	goose_result result_count;

	REQUIRE(goose_open(NULL, &db) != GooseError);
	REQUIRE(goose_connect(db, &con) != GooseError);

	REQUIRE(goose_query(con, "CREATE TABLE integers(i INTEGER, j INTEGER);", NULL) != GooseError);
	REQUIRE((goose_query(con, "SELECT count(*) FROM integers;", &result_count) != GooseError));

	goose_destroy_result(&result_count);

	REQUIRE(goose_query(con, "non valid SQL", &result) == GooseError);

	goose_destroy_result(&result); // segmentation failure happens here

	goose_disconnect(&con);
	goose_close(&db);
}

TEST_CASE("Decimal -> Double casting issue", "[capi]") {
	CAPITester tester;
	goose::unique_ptr<CAPIResult> result;

	// open the database in in-memory mode
	REQUIRE(tester.OpenDatabase(nullptr));

	result = tester.Query("select -0.5;");
	REQUIRE_NO_FAIL(*result);

	REQUIRE(result->ColumnType(0) == GOOSE_TYPE_DECIMAL);
	auto double_from_decimal = result->Fetch<double>(0, 0);
	REQUIRE(double_from_decimal == (double)-0.5);

	auto string_from_decimal = result->Fetch<string>(0, 0);
	REQUIRE(string_from_decimal == "-0.5");
}

TEST_CASE("Test custom_user_agent config", "[capi]") {
	{
		goose_database db;
		goose_connection con;
		goose_result result;

		// Default custom_user_agent value
		REQUIRE(goose_open_ext(NULL, &db, nullptr, NULL) != GooseError);
		REQUIRE(goose_connect(db, &con) != GooseError);

		goose_query(con, "PRAGMA user_agent", &result);

		REQUIRE(goose_row_count(&result) == 1);
		char *user_agent_value = goose_value_varchar(&result, 0, 0);
		REQUIRE_THAT(user_agent_value, Catch::Matchers::Matches("goose/.*(.*) capi"));

		goose_free(user_agent_value);
		goose_destroy_result(&result);
		goose_disconnect(&con);
		goose_close(&db);
	}

	{
		// Custom custom_user_agent value

		goose_database db;
		goose_connection con;
		goose_result result_custom_user_agent;
		goose_result result_full_user_agent;

		goose_config config;
		REQUIRE(goose_create_config(&config) != GooseError);
		REQUIRE(goose_set_config(config, "custom_user_agent", "CUSTOM_STRING") != GooseError);

		REQUIRE(goose_open_ext(NULL, &db, config, NULL) != GooseError);
		REQUIRE(goose_connect(db, &con) != GooseError);

		goose_query(con, "SELECT current_setting('custom_user_agent')", &result_custom_user_agent);
		goose_query(con, "PRAGMA user_agent", &result_full_user_agent);

		REQUIRE(goose_row_count(&result_custom_user_agent) == 1);
		REQUIRE(goose_row_count(&result_full_user_agent) == 1);

		char *custom_user_agent_value = goose_value_varchar(&result_custom_user_agent, 0, 0);
		REQUIRE(string(custom_user_agent_value) == "CUSTOM_STRING");

		char *full_user_agent_value = goose_value_varchar(&result_full_user_agent, 0, 0);
		REQUIRE_THAT(full_user_agent_value, Catch::Matchers::Matches("goose/.*(.*) capi CUSTOM_STRING"));

		goose_destroy_config(&config);
		goose_free(custom_user_agent_value);
		goose_free(full_user_agent_value);
		goose_destroy_result(&result_custom_user_agent);
		goose_destroy_result(&result_full_user_agent);
		goose_disconnect(&con);
		goose_close(&db);
	}
}

TEST_CASE("Test unsupported types in the deprecated C API", "[capi]") {
	CAPITester tester;
	REQUIRE(tester.OpenDatabase(nullptr));

	string query_1 = R"EOF(
		CREATE TABLE test(
			id BIGINT,
			one DECIMAL(18,3)[]
		);
	)EOF";
	string query_2 = "INSERT INTO test VALUES (410, '[]');";
	string query_3 = "INSERT INTO test VALUES (412, '[]');";
	string query_4 = "SELECT id, one FROM test;";
	REQUIRE_NO_FAIL(tester.Query(query_1));
	REQUIRE_NO_FAIL(tester.Query(query_2));
	REQUIRE_NO_FAIL(tester.Query(query_3));

	// Passes, but does return invalid data for unsupported types.
	auto result = tester.Query(query_4);
	auto &result_c = result->InternalResult();

	auto first_bigint_row = goose_value_string(&result_c, 0, 0).data;
	REQUIRE(!string(first_bigint_row).compare("410"));
	goose_free(first_bigint_row);
	REQUIRE(goose_value_string(&result_c, 1, 0).data == nullptr);

	auto second_bigint_row = goose_value_string(&result_c, 0, 1).data;
	REQUIRE(!string(second_bigint_row).compare("412"));
	goose_free(second_bigint_row);
	REQUIRE(goose_value_string(&result_c, 1, 1).data == nullptr);
}
