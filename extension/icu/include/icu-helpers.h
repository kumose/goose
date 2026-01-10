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

#include <goose/goose.h>
#include "unicode/timezone.h"
#include <goose/common/types/timestamp.h>

namespace goose {

struct ICUHelpers {
	//! Tries to get a time zone - returns nullptr if the timezone is not found
	static unique_ptr<icu::TimeZone> TryGetTimeZone(string &tz_str);
	//! Gets a time zone - throws an error if the timezone is not found
	static unique_ptr<icu::TimeZone> GetTimeZone(string &tz_str, string *error_message = nullptr);

	static TimestampComponents GetComponents(timestamp_tz_t ts, icu::Calendar *calendar);

	static timestamp_t ToTimestamp(TimestampComponents data);
};

} // namespace goose
