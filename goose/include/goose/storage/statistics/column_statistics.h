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
