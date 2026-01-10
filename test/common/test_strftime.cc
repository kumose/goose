#include "catch.h"
#include <goose/function/scalar/strftime_format.h>
#include <goose/common/types/timestamp.h>
#include <goose/common/types/date.h>

#include <vector>

using namespace goose;
using namespace std;

TEST_CASE("Test that strftime format works", "[strftime]") {
	auto string = StrfTimeFormat::Format(Timestamp::FromDatetime(Date::FromDate(1992, 1, 1), dtime_t(0)), "%Y%m%d");
	REQUIRE(string == "19920101");
}
