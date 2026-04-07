#pragma once

namespace goose {

//===--------------------------------------------------------------------===//
// Arrow Time/Date Types
//===--------------------------------------------------------------------===//
enum class ArrowDateTimeType : uint8_t {
	MILLISECONDS,
	MICROSECONDS,
	NANOSECONDS,
	SECONDS,
	DAYS,
	MONTHS,
	MONTH_DAY_NANO
};

} // namespace goose
