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

#include <goose/goose.h>
#include <xicu/unicode/timezone.h>
#include <goose/common/types/timestamp.h>

namespace goose {

struct ICUHelpers {
	//! Tries to get a time zone - returns nullptr if the timezone is not found
	static unique_ptr<xicu::TimeZone> TryGetTimeZone(string &tz_str);
	//! Gets a time zone - throws an error if the timezone is not found
	static unique_ptr<xicu::TimeZone> GetTimeZone(string &tz_str, string *error_message = nullptr);

	static TimestampComponents GetComponents(timestamp_tz_t ts, xicu::Calendar *calendar);

	static timestamp_t ToTimestamp(TimestampComponents data);
};

} // namespace goose
