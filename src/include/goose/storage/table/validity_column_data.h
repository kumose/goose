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
