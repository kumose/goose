// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <goose/common/common.h>
#include <goose/common/types.h>
#include <goose/common/winapi.h>

namespace goose {

struct dtime_t;    // NOLINT
struct dtime_tz_t; // NOLINT

//! The Time class is a static class that holds helper functions for the Time
//! type.
class Time {
public:
	//! Convert a string in the format "hh:mm:ss" to a time object
	GOOSE_API static dtime_t FromString(const string &str, bool strict = false, optional_ptr<int32_t> nanos = nullptr);
	GOOSE_API static dtime_t FromCString(const char *buf, idx_t len, bool strict = false,
	                                      optional_ptr<int32_t> nanos = nullptr);
	GOOSE_API static bool TryConvertTime(const char *buf, idx_t len, idx_t &pos, dtime_t &result, bool strict = false,
	                                      optional_ptr<int32_t> nanos = nullptr);
	GOOSE_API static bool TryConvertTimeTZ(const char *buf, idx_t len, idx_t &pos, dtime_tz_t &result,
	                                        bool &has_offset, bool strict = false,
	                                        optional_ptr<int32_t> nanos = nullptr);
	// No hour limit
	GOOSE_API static bool TryConvertInterval(const char *buf, idx_t len, idx_t &pos, dtime_t &result,
	                                          bool strict = false, optional_ptr<int32_t> nanos = nullptr);

	//! Convert a time object to a string in the format "hh:mm:ss"
	GOOSE_API static string ToString(dtime_t time);
	//! Convert a UTC offset to ±HH[:MM]
	GOOSE_API static string ToUTCOffset(int hour_offset, int minute_offset);

	GOOSE_API static dtime_t FromTime(int32_t hour, int32_t minute, int32_t second, int32_t microseconds = 0);
	GOOSE_API static int64_t ToNanoTime(int32_t hour, int32_t minute, int32_t second, int32_t nanoseconds = 0);

	//! Normalize a TIME_TZ by adding the offset to the time part and returning the TIME
	GOOSE_API static dtime_t NormalizeTimeTZ(dtime_tz_t timetz);

	//! Extract the time from a given timestamp object
	GOOSE_API static void Convert(dtime_t time, int32_t &out_hour, int32_t &out_min, int32_t &out_sec,
	                               int32_t &out_micros);

	GOOSE_API static string ConversionError(const string &str);
	GOOSE_API static string ConversionError(string_t str);

	GOOSE_API static dtime_t FromTimeMs(int64_t time_ms);
	GOOSE_API static dtime_t FromTimeNs(int64_t time_ns);

	GOOSE_API static bool IsValidTime(int32_t hour, int32_t minute, int32_t second, int32_t microseconds);

private:
	static bool TryConvertInternal(const char *buf, idx_t len, idx_t &pos, dtime_t &result, bool strict,
	                               optional_ptr<int32_t> nanos = nullptr);
};

} // namespace goose
