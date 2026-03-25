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

#include <goose/extension/icu/icu-datefunc.h>

namespace goose {

struct ICUMakeDate : public ICUDateFunc {
	static date_t Operation(xicu::Calendar *calendar, timestamp_t instant);

	static bool CastToDate(Vector &source, Vector &result, idx_t count, CastParameters &parameters);

	static BoundCastInfo BindCastToDate(BindCastInput &input, const LogicalType &source, const LogicalType &target);

	static void AddCasts(ExtensionLoader &loader);

	static date_t ToDate(ClientContext &context, timestamp_t instant);
};

struct ICUToTimeTZ : public ICUDateFunc {
	static dtime_tz_t Operation(xicu::Calendar *calendar, dtime_tz_t timetz);

	static bool ToTimeTZ(xicu::Calendar *calendar, timestamp_t instant, dtime_tz_t &result);

	static bool CastToTimeTZ(Vector &source, Vector &result, idx_t count, CastParameters &parameters);

	static BoundCastInfo BindCastToTimeTZ(BindCastInput &input, const LogicalType &source, const LogicalType &target);

	static void AddCasts(ExtensionLoader &loader);
};

} // namespace goose
