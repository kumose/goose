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

namespace goose {

//! Validity column data represents the validity data (i.e. which values are null)
class ValidityColumnData : public ColumnData {
	friend class StandardColumnData;

public:
	ValidityColumnData(BlockManager &block_manager, DataTableInfo &info, idx_t column_index, ColumnData &parent);
	ValidityColumnData(BlockManager &block_manager, DataTableInfo &info, idx_t column_index, ColumnDataType data_type,
	                   optional_ptr<ColumnData> parent);

public:
	FilterPropagateResult CheckZonemap(ColumnScanState &state, TableFilter &filter) override;
	void AppendData(BaseStatistics &stats, ColumnAppendState &state, UnifiedVectorFormat &vdata, idx_t count) override;
	unique_ptr<ColumnCheckpointState> CreateCheckpointState(const RowGroup &row_group,
	                                                        PartialBlockManager &partial_block_manager) override;

	void Verify(RowGroup &parent) override;
	void UpdateWithBase(TransactionData transaction, DataTable &data_table, idx_t column_index, Vector &update_vector,
	                    row_t *row_ids, idx_t update_count, ColumnData &base, idx_t row_group_start);
};

} // namespace goose
