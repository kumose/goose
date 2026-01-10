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

#include <goose/common/constants.h>

namespace goose {

enum class DatePartSpecifier : uint8_t {
	//	BIGINT values
	YEAR,
	MONTH,
	DAY,
	DECADE,
	CENTURY,
	MILLENNIUM,
	MICROSECONDS,
	MILLISECONDS,
	SECOND,
	MINUTE,
	HOUR,
	DOW,
	ISODOW,
	WEEK,
	ISOYEAR,
	QUARTER,
	DOY,
	YEARWEEK,
	ERA,
	TIMEZONE,
	TIMEZONE_HOUR,
	TIMEZONE_MINUTE,

	//	DOUBLE values
	EPOCH,
	JULIAN_DAY,

	//	Invalid
	INVALID,

	//	Type ranges
	BEGIN_BIGINT = YEAR,
	BEGIN_DOUBLE = EPOCH,
	BEGIN_INVALID = INVALID,
};

inline bool IsBigintDatepart(DatePartSpecifier part_code) {
	return size_t(part_code) < size_t(DatePartSpecifier::BEGIN_DOUBLE);
}

GOOSE_API bool TryGetDatePartSpecifier(const string &specifier, DatePartSpecifier &result);
GOOSE_API DatePartSpecifier GetDatePartSpecifier(const string &specifier);

} // namespace goose
