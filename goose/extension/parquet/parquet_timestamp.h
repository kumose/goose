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

namespace goose {

struct Int96 {
	uint32_t value[3];
};

timestamp_t ImpalaTimestampToTimestamp(const Int96 &raw_ts);
timestamp_ns_t ImpalaTimestampToTimestampNS(const Int96 &raw_ts);
Int96 TimestampToImpalaTimestamp(timestamp_t &ts);

timestamp_t ParquetTimestampMicrosToTimestamp(const int64_t &raw_ts);
timestamp_t ParquetTimestampMsToTimestamp(const int64_t &raw_ts);
timestamp_t ParquetTimestampNsToTimestamp(const int64_t &raw_ts);

timestamp_ns_t ParquetTimestampMsToTimestampNs(const int64_t &raw_ms);
timestamp_ns_t ParquetTimestampUsToTimestampNs(const int64_t &raw_us);
timestamp_ns_t ParquetTimestampNsToTimestampNs(const int64_t &raw_ns);

date_t ParquetIntToDate(const int32_t &raw_date);
dtime_t ParquetMsIntToTime(const int32_t &raw_millis);
dtime_t ParquetIntToTime(const int64_t &raw_micros);
dtime_t ParquetNsIntToTime(const int64_t &raw_nanos);

dtime_ns_t ParquetMsIntToTimeNs(const int32_t &raw_millis);
dtime_ns_t ParquetUsIntToTimeNs(const int64_t &raw_micros);
dtime_ns_t ParquetIntToTimeNs(const int64_t &raw_nanos);

dtime_tz_t ParquetIntToTimeMsTZ(const int32_t &raw_millis);
dtime_tz_t ParquetIntToTimeTZ(const int64_t &raw_micros);
dtime_tz_t ParquetIntToTimeNsTZ(const int64_t &raw_nanos);

} // namespace goose
