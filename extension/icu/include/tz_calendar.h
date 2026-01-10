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

#include "unicode/calendar.h"
#include <goose/common/string_util.h>

namespace goose {

using CalendarPtr = goose::unique_ptr<icu::Calendar>;

struct TZCalendar {
	TZCalendar(icu::Calendar &calendar_p, const string &cal_setting)
	    : calendar(CalendarPtr(calendar_p.clone())),
	      is_gregorian(cal_setting.empty() || StringUtil::CIEquals(cal_setting, "gregorian")),
	      supports_intervals(calendar->getMaximum(UCAL_MONTH) < 12) { // 0-based
	}

	icu::Calendar *GetICUCalendar() {
		return calendar.get();
	}
	bool IsGregorian() const {
		return is_gregorian;
	}
	bool SupportsIntervals() const {
		return supports_intervals;
	}

	CalendarPtr calendar;
	const bool is_gregorian;
	const bool supports_intervals;
};

} // namespace goose
