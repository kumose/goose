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

#include <goose/common/types/column/partitioned_column_data.h>
#include <goose/execution/expression_executor.h>
#include <goose/optimizer/filter_combiner.h>
#include <goose/optimizer/statistics_propagator.h>
#include <goose/planner/expression_iterator.h>
#include <goose/planner/table_filter.h>
#include <goose/common/open_file_info.h>
#include <goose/common/types-import.h>

#include <iostream>

namespace goose {
struct MultiFilePushdownInfo;

struct HivePartitioningFilterInfo {
	unordered_map<string, column_t> column_map;
	bool hive_enabled;
	bool filename_enabled;
};

class HivePartitioning {
public:
	//! Parse a filename that follows the hive partitioning scheme
	GOOSE_API static std::map<string, string> Parse(const string &filename);
	//! Prunes a list of filenames based on a set of filters, can be used by TableFunctions in the
	//! pushdown_complex_filter function to skip files with filename-based filters. Also removes the filters that always
	//! evaluate to true.
	GOOSE_API static void ApplyFiltersToFileList(ClientContext &context, vector<OpenFileInfo> &files,
	                                              vector<unique_ptr<Expression>> &filters,
	                                              const HivePartitioningFilterInfo &filter_info,
	                                              MultiFilePushdownInfo &info);

	GOOSE_API static Value GetValue(ClientContext &context, const string &key, const string &value,
	                                 const LogicalType &type);
	//! Escape a hive partition key or value using URL encoding
	GOOSE_API static string Escape(const string &input);
	//! Unescape a hive partition key or value encoded using URL encoding
	GOOSE_API static string Unescape(const string &input);
};

struct HivePartitionKey {
	//! Columns by which we want to partition
	vector<Value> values;
	//! Precomputed hash of values
	hash_t hash;

	struct Hash {
		std::size_t operator()(const HivePartitionKey &k) const {
			return k.hash;
		}
	};

	struct Equality {
		bool operator()(const HivePartitionKey &a, const HivePartitionKey &b) const {
			if (a.values.size() != b.values.size()) {
				return false;
			}
			for (idx_t i = 0; i < a.values.size(); i++) {
				if (!Value::NotDistinctFrom(a.values[i], b.values[i])) {
					return false;
				}
			}
			return true;
		}
	};
};

//! Maps hive partitions to partition_ids
typedef unordered_map<HivePartitionKey, idx_t, HivePartitionKey::Hash, HivePartitionKey::Equality> hive_partition_map_t;

//! class shared between HivePartitionColumnData classes that synchronizes partition discovery between threads.
//! each HivePartitionedColumnData will hold a local copy of the key->partition map
class GlobalHivePartitionState {
public:
	mutex lock;
	hive_partition_map_t partition_map;
};

class HivePartitionedColumnData : public PartitionedColumnData {
public:
	HivePartitionedColumnData(ClientContext &context, vector<LogicalType> types, vector<idx_t> partition_by_cols,
	                          shared_ptr<GlobalHivePartitionState> global_state = nullptr);
	void ComputePartitionIndices(PartitionedColumnDataAppendState &state, DataChunk &input) override;

	//! Reverse lookup map to reconstruct keys from a partition id
	std::map<idx_t, const HivePartitionKey *> GetReverseMap();

protected:
	//! Register a newly discovered partition
	idx_t RegisterNewPartition(HivePartitionKey key, PartitionedColumnDataAppendState &state);
	//! Add a new partition with the given partition id
	void AddNewPartition(HivePartitionKey key, idx_t partition_id, PartitionedColumnDataAppendState &state);

private:
	void InitializeKeys();

protected:
	//! Shared HivePartitionedColumnData should always have a global state to allow parallel key discovery
	shared_ptr<GlobalHivePartitionState> global_state;
	//! Thread-local copy of the partition map
	hive_partition_map_t local_partition_map;
	//! The columns that make up the key
	vector<idx_t> group_by_columns;
	//! Thread-local pre-allocated vector for hashes
	Vector hashes_v;
	//! Thread-local pre-allocated HivePartitionKeys
	vector<HivePartitionKey> keys;
};

} // namespace goose
