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

#include <goose/storage/statistics/base_statistics.h>
#include <goose/storage/statistics/distinct_statistics.h>

namespace goose {
class Serializer;

class ColumnStatistics {
public:
	explicit ColumnStatistics(BaseStatistics stats_p);
	ColumnStatistics(BaseStatistics stats_p, unique_ptr<DistinctStatistics> distinct_stats_p);

public:
	static shared_ptr<ColumnStatistics> CreateEmptyStats(const LogicalType &type);

	void Merge(ColumnStatistics &other);

	void UpdateDistinctStatistics(Vector &v, idx_t count, Vector &hashes);

	BaseStatistics &Statistics();

	bool HasDistinctStats();
	DistinctStatistics &DistinctStats();
	void SetDistinct(unique_ptr<DistinctStatistics> distinct_stats);

	shared_ptr<ColumnStatistics> Copy() const;

	void Serialize(Serializer &serializer) const;
	static shared_ptr<ColumnStatistics> Deserialize(Deserializer &source);

private:
	BaseStatistics stats;
	//! The approximate count distinct stats of the column
	unique_ptr<DistinctStatistics> distinct_stats;
};

} // namespace goose
