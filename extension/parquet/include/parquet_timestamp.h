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
