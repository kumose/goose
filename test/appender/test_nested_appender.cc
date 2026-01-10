#include "catch.h"
#include <goose/main/appender.h>
#include "test_helpers.h"

#include <vector>

using namespace goose;
using namespace std;

TEST_CASE("Test appender with lists", "[appender]") {
	goose::unique_ptr<QueryResult> result;
	Goose db(nullptr);
	Connection con(db);

	REQUIRE_NO_FAIL(con.Query("CREATE TABLE lists(i INTEGER[])"));

	// append a value to the table
	Appender appender(con, "lists");

	auto list_value = Value::LIST(LogicalType::INTEGER, {Value::INTEGER(3)});
	auto empty_list_value = Value::LIST(LogicalType::INTEGER, goose::vector<Value>());

	appender.AppendRow(list_value);
	appender.AppendRow(empty_list_value);

	appender.Close();

	// we can select the value now
	result = con.Query("SELECT * FROM lists");
	REQUIRE(CHECK_COLUMN(result, 0, {list_value, empty_list_value}));
}

TEST_CASE("Test appender with nested lists", "[appender]") {
	goose::unique_ptr<QueryResult> result;
	Goose db(nullptr);
	Connection con(db);

	REQUIRE_NO_FAIL(con.Query("CREATE TABLE lists(i INTEGER[][])"));

	// append a value to the table
	Appender appender(con, "lists");

	auto int_list_type = LogicalType::LIST(LogicalType::INTEGER);
	auto nested_int_list_type = LogicalType::LIST(int_list_type);
	auto list_value =
	    Value::LIST(int_list_type, {Value::LIST(LogicalType::INTEGER, {Value::INTEGER(1)}),
	                                Value::LIST(LogicalType::INTEGER, {Value::INTEGER(2), Value::INTEGER(3)})});
	auto empty_list_value = Value::LIST(int_list_type, goose::vector<Value>());
	auto null_list_value = Value(nested_int_list_type);
	appender.AppendRow(list_value);
	appender.AppendRow(empty_list_value);
	appender.AppendRow(null_list_value);

	appender.Close();

	// we can select the value now
	result = con.Query("SELECT * FROM lists");
	REQUIRE(CHECK_COLUMN(result, 0, {list_value, empty_list_value, null_list_value}));
}
