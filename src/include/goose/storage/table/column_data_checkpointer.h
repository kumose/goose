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

#include <goose/storage/table/column_data.h>
#include <goose/function/compression_function.h>
#include <goose/storage/table/column_checkpoint_state.h>

namespace goose {
struct TableScanOptions;

//! Holds state related to a single column during compression
struct ColumnDataCheckpointData {
public:
	//! Default constructor used when column data does not need to be checkpointed
	ColumnDataCheckpointData() {
	}
	ColumnDataCheckpointData(ColumnCheckpointState &checkpoint_state, ColumnData &col_data, DatabaseInstance &db,
	                         const RowGroup &row_group, StorageManager &storage_manager)
	    : checkpoint_state(checkpoint_state), col_data(col_data), db(db), row_group(row_group),
	      storage_manager(storage_manager) {
	}

public:
	CompressionFunction &GetCompressionFunction(CompressionType type);
	const LogicalType &GetType() const;
	ColumnData &GetColumnData();
	const RowGroup &GetRowGroup();
	ColumnCheckpointState &GetCheckpointState();
	DatabaseInstance &GetDatabase();
	StorageManager &GetStorageManager();

private:
	optional_ptr<ColumnCheckpointState> checkpoint_state;
	optional_ptr<ColumnData> col_data;
	optional_ptr<DatabaseInstance> db;
	optional_ptr<const RowGroup> row_group;
	optional_ptr<StorageManager> storage_manager;
};

struct CheckpointAnalyzeResult {
public:
	//! Default constructor, returned when the column data doesn't require checkpoint
	CheckpointAnalyzeResult() {
	}
	CheckpointAnalyzeResult(unique_ptr<AnalyzeState> &&analyze_state, CompressionFunction &function)
	    : analyze_state(std::move(analyze_state)), function(function) {
	}

public:
	unique_ptr<AnalyzeState> analyze_state;
	optional_ptr<CompressionFunction> function;
};

class ColumnDataCheckpointer {
public:
	ColumnDataCheckpointer(vector<reference<ColumnCheckpointState>> &states, StorageManager &storage_manager,
	                       const RowGroup &row_group, ColumnCheckpointInfo &checkpoint_info);

public:
	void Checkpoint();
	void FinalizeCheckpoint();

private:
	void ScanSegments(const std::function<void(Vector &, idx_t)> &callback);
	vector<CheckpointAnalyzeResult> DetectBestCompressionMethod();
	void WriteToDisk();
	void WritePersistentSegments(ColumnCheckpointState &state);
	bool HasChanges(ColumnData &col_data);
	void InitAnalyze();
	void DropSegments();
	bool ValidityCoveredByBasedata(vector<CheckpointAnalyzeResult> &result);

private:
	vector<reference<ColumnCheckpointState>> &checkpoint_states;
	StorageManager &storage_manager;
	const RowGroup &row_group;
	Vector intermediate;
	ColumnCheckpointInfo &checkpoint_info;

	bool has_changes = false;
	//! For every column data that is being checkpointed, the applicable functions
	vector<vector<optional_ptr<CompressionFunction>>> compression_functions;
	//! For every column data that is being checkpointed, the analyze state of functions being tried
	vector<vector<unique_ptr<AnalyzeState>>> analyze_states;
};

} // namespace goose
