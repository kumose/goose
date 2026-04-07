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

#include <goose/storage/table/column_data.h>
#include <goose/storage/table/validity_column_data.h>

namespace goose {

//! Standard column data represents a regular flat column (e.g. a column of type INTEGER or STRING)
class StandardColumnData : public ColumnData {
public:
	StandardColumnData(BlockManager &block_manager, DataTableInfo &info, idx_t column_index, LogicalType type,
	                   ColumnDataType data_type, optional_ptr<ColumnData> parent);

public:
	void SetDataType(ColumnDataType data_type) override;

	ScanVectorType GetVectorScanType(ColumnScanState &state, idx_t scan_count, Vector &result) override;
	void InitializePrefetch(PrefetchState &prefetch_state, ColumnScanState &scan_state, idx_t rows) override;
	void InitializeScan(ColumnScanState &state) override;
	void InitializeScanWithOffset(ColumnScanState &state, idx_t row_idx) override;

	idx_t Scan(TransactionData transaction, idx_t vector_index, ColumnScanState &state, Vector &result,
	           idx_t target_count) override;
	idx_t ScanCommitted(idx_t vector_index, ColumnScanState &state, Vector &result, bool allow_updates,
	                    idx_t target_count) override;
	idx_t ScanCount(ColumnScanState &state, Vector &result, idx_t count, idx_t result_offset) override;

	void Filter(TransactionData transaction, idx_t vector_index, ColumnScanState &state, Vector &result,
	            SelectionVector &sel, idx_t &count, const TableFilter &filter, TableFilterState &filter_state) override;
	void Select(TransactionData transaction, idx_t vector_index, ColumnScanState &state, Vector &result,
	            SelectionVector &sel, idx_t sel_count) override;

	void InitializeAppend(ColumnAppendState &state) override;
	void AppendData(BaseStatistics &stats, ColumnAppendState &state, UnifiedVectorFormat &vdata, idx_t count) override;
	void RevertAppend(row_t new_count) override;
	idx_t Fetch(ColumnScanState &state, row_t row_id, Vector &result) override;
	void FetchRow(TransactionData transaction, ColumnFetchState &state, const StorageIndex &storage_index, row_t row_id,
	              Vector &result, idx_t result_idx) override;
	void Update(TransactionData transaction, DataTable &data_table, idx_t column_index, Vector &update_vector,
	            row_t *row_ids, idx_t update_count, idx_t row_group_start) override;
	void UpdateColumn(TransactionData transaction, DataTable &data_table, const vector<column_t> &column_path,
	                  Vector &update_vector, row_t *row_ids, idx_t update_count, idx_t depth,
	                  idx_t row_group_start) override;
	unique_ptr<BaseStatistics> GetUpdateStatistics() override;

	void VisitBlockIds(BlockIdVisitor &visitor) const override;

	unique_ptr<ColumnCheckpointState> CreateCheckpointState(const RowGroup &row_group,
	                                                        PartialBlockManager &partial_block_manager) override;
	unique_ptr<ColumnCheckpointState> Checkpoint(const RowGroup &row_group, ColumnCheckpointInfo &info) override;
	void CheckpointScan(ColumnSegment &segment, ColumnScanState &state, idx_t count,
	                    Vector &scan_vector) const override;

	void GetColumnSegmentInfo(const QueryContext &context, goose::idx_t row_group_index,
	                          vector<goose::idx_t> col_path, vector<goose::ColumnSegmentInfo> &result) override;

	bool IsPersistent() override;
	bool HasAnyChanges() const override;
	PersistentColumnData Serialize() override;
	void InitializeColumn(PersistentColumnData &column_data, BaseStatistics &target_stats) override;

	void Verify(RowGroup &parent) override;

	void SetValidityData(shared_ptr<ValidityColumnData> validity);

protected:
	//! The validity column data
	shared_ptr<ValidityColumnData> validity;
};

} // namespace goose
