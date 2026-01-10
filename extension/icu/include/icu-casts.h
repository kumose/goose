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

#include "icu-datefunc.h"

namespace goose {

struct ICUMakeDate : public ICUDateFunc {
	static date_t Operation(icu::Calendar *calendar, timestamp_t instant);

	static bool CastToDate(Vector &source, Vector &result, idx_t count, CastParameters &parameters);

	static BoundCastInfo BindCastToDate(BindCastInput &input, const LogicalType &source, const LogicalType &target);

	static void AddCasts(ExtensionLoader &loader);

	static date_t ToDate(ClientContext &context, timestamp_t instant);
};

struct ICUToTimeTZ : public ICUDateFunc {
	static dtime_tz_t Operation(icu::Calendar *calendar, dtime_tz_t timetz);

	static bool ToTimeTZ(icu::Calendar *calendar, timestamp_t instant, dtime_tz_t &result);

	static bool CastToTimeTZ(Vector &source, Vector &result, idx_t count, CastParameters &parameters);

	static BoundCastInfo BindCastToTimeTZ(BindCastInput &input, const LogicalType &source, const LogicalType &target);

	static void AddCasts(ExtensionLoader &loader);
};

} // namespace goose
