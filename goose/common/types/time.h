// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
