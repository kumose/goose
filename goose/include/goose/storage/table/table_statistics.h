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

#include <goose/common/common.h>
#include <goose/common/types/data_chunk.h>
#include <goose/execution/reservoir_sample.h>
#include <goose/common/types-import.h>
#include <goose/storage/statistics/column_statistics.h>
#include <goose/storage/storage_index.h>

namespace goose {
class ColumnList;
class PersistentTableData;
class Serializer;
class Deserializer;

class TableStatisticsLock {
public:
	explicit TableStatisticsLock(mutex &l) : guard(l) {
	}

	lock_guard<mutex> guard;
};

class TableStatistics {
public:
	void Initialize(const vector<LogicalType> &types, PersistentTableData &data);
	void InitializeEmpty(const vector<LogicalType> &types);
	void InitializeEmpty(const TableStatistics &other);

	void InitializeAddColumn(TableStatistics &parent, const LogicalType &new_column_type);
	void InitializeRemoveColumn(TableStatistics &parent, idx_t removed_column);
	void InitializeAlterType(TableStatistics &parent, idx_t changed_idx, const LogicalType &new_type);
	void InitializeAddConstraint(TableStatistics &parent);

	void MergeStats(TableStatistics &other);
	void MergeStats(idx_t i, BaseStatistics &stats);
	void MergeStats(TableStatisticsLock &lock, idx_t i, BaseStatistics &stats);

	void SetStats(TableStatistics &other);
	void CopyStats(TableStatistics &other);
	void CopyStats(TableStatisticsLock &lock, TableStatistics &other);
	unique_ptr<BaseStatistics> CopyStats(const StorageIndex &i);
	//! Get a reference to the stats - this requires us to hold the lock.
	//! The reference can only be safely accessed while the lock is held
	ColumnStatistics &GetStats(TableStatisticsLock &lock, idx_t i);
	//! Get a reference to the table sample - this requires us to hold the lock.
	// BlockingSample &GetTableSampleRef(TableStatisticsLock &lock);
	//! Take ownership of the sample, needed for merging. Requires the lock
	unique_ptr<BlockingSample> GetTableSample(TableStatisticsLock &lock);
	void SetTableSample(TableStatisticsLock &lock, unique_ptr<BlockingSample> sample);

	void DestroyTableSample(TableStatisticsLock &lock) const;
	void AppendToTableSample(TableStatisticsLock &lock, unique_ptr<BlockingSample> sample);

	bool Empty();

	unique_ptr<TableStatisticsLock> GetLock();

	void Serialize(Serializer &serializer) const;
	void Deserialize(Deserializer &deserializer, ColumnList &columns);

private:
	//! The statistics lock
	shared_ptr<mutex> stats_lock;
	//! Column statistics
	vector<shared_ptr<ColumnStatistics>> column_stats;
	//! The table sample
	unique_ptr<BlockingSample> table_sample;
};

} // namespace goose
