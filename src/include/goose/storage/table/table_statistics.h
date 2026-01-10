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
