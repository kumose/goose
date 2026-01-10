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

#include <goose/common/types/data_chunk.h>
#include <goose/storage/statistics/base_statistics.h>
#include <goose/storage/data_pointer.h>
#include <goose/storage/statistics/segment_statistics.h>
#include <goose/storage/table/column_segment.h>
#include <goose/storage/table/column_data.h>
#include <goose/common/types-import.h>
#include <goose/storage/partial_block_manager.h>

namespace goose {
class ColumnData;
class DatabaseInstance;
class RowGroup;
class PartialBlockManager;
class TableDataWriter;

struct ColumnCheckpointState {
	ColumnCheckpointState(const RowGroup &row_group, ColumnData &original_column,
	                      PartialBlockManager &partial_block_manager);
	virtual ~ColumnCheckpointState();

	const RowGroup &row_group;
	const ColumnData &original_column;
	vector<DataPointer> data_pointers;
	unique_ptr<BaseStatistics> global_stats;

protected:
	PartialBlockManager &partial_block_manager;
	shared_ptr<ColumnData> result_column;

private:
	ColumnData &original_column_mutable;

public:
	virtual shared_ptr<ColumnData> CreateEmptyColumnData();
	virtual ColumnData &GetResultColumn();
	virtual shared_ptr<ColumnData> GetFinalResult();

	virtual unique_ptr<BaseStatistics> GetStatistics();

	virtual void FlushSegmentInternal(unique_ptr<ColumnSegment> segment, idx_t segment_size);
	virtual void FlushSegment(unique_ptr<ColumnSegment> segment, BufferHandle handle, idx_t segment_size);
	virtual PersistentColumnData ToPersistentData();

	PartialBlockManager &GetPartialBlockManager() {
		return partial_block_manager;
	}

public:
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

struct PartialBlockForCheckpoint : public PartialBlock {
	struct PartialColumnSegment {
		PartialColumnSegment(ColumnData &data, ColumnSegment &segment, uint32_t offset_in_block)
		    : data(data), segment(segment), offset_in_block(offset_in_block) {
		}

		ColumnData &data;
		ColumnSegment &segment;
		uint32_t offset_in_block;
	};

public:
	PartialBlockForCheckpoint(ColumnData &data, ColumnSegment &segment, PartialBlockState state,
	                          BlockManager &block_manager);
	~PartialBlockForCheckpoint() override;

	// We will copy all segment data into the memory of the shared block.
	// Once the block is full (or checkpoint is complete) we'll invoke Flush().
	// This will cause the block to get written to storage (via BlockManger::ConvertToPersistent),
	// and all segments to have their references updated (via ColumnSegment::ConvertToPersistent)
	vector<PartialColumnSegment> segments;

public:
	bool IsFlushed();
	void Flush(QueryContext context, const idx_t free_space_left) override;
	void Merge(PartialBlock &other, idx_t offset, idx_t other_size) override;
	void AddSegmentToTail(ColumnData &data, ColumnSegment &segment, uint32_t offset_in_block) override;
	void Clear() override;
};

} // namespace goose
