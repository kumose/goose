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

#include <goose/common/enums/scan_vector_type.h>
#include <goose/common/types.h>
#include <goose/common/types/vector.h>
#include <goose/function/compression_function.h>
#include <goose/storage/block.h>
#include <goose/storage/buffer/block_handle.h>
#include <goose/storage/buffer_manager.h>
#include <goose/storage/statistics/segment_statistics.h>
#include <goose/storage/storage_lock.h>
#include <goose/storage/table/segment_base.h>

namespace goose {

class BaseStatistics;
class BlockManager;
class ColumnData;
class ColumnSegment;
class DatabaseInstance;
class TableFilter;
class Transaction;
class UpdateSegment;
struct ColumnAppendState;
struct ColumnFetchState;
struct ColumnScanState;
struct PrefetchState;
struct TableFilterState;

enum class ColumnSegmentType : uint8_t { TRANSIENT, PERSISTENT };
//! TableFilter represents a filter pushed down into the table scan.

class ColumnSegment : public SegmentBase<ColumnSegment> {
public:
	//! Construct a column segment.
	ColumnSegment(DatabaseInstance &db, shared_ptr<BlockHandle> block, const LogicalType &type,
	              const ColumnSegmentType segment_type, const idx_t count, CompressionFunction &function_p,
	              BaseStatistics statistics, const block_id_t block_id_p, const idx_t offset,
	              const idx_t segment_size_p, unique_ptr<ColumnSegmentState> segment_state_p = nullptr);
	//! Construct a column segment from another column segment.
	//! The other column segment becomes invalid (std::move).
	ColumnSegment(ColumnSegment &other);
	~ColumnSegment();

public:
	static unique_ptr<ColumnSegment> CreatePersistentSegment(DatabaseInstance &db, BlockManager &block_manager,
	                                                         block_id_t id, idx_t offset, const LogicalType &type_p,
	                                                         idx_t count, CompressionType compression_type,
	                                                         BaseStatistics statistics,
	                                                         unique_ptr<ColumnSegmentState> segment_state);
	static unique_ptr<ColumnSegment> CreateTransientSegment(DatabaseInstance &db, CompressionFunction &function,
	                                                        const LogicalType &type, const idx_t segment_size,
	                                                        BlockManager &block_manager);

public:
	void InitializePrefetch(PrefetchState &prefetch_state, ColumnScanState &scan_state);
	void InitializeScan(ColumnScanState &state);
	//! Scan one vector from this segment
	void Scan(ColumnScanState &state, idx_t scan_count, Vector &result, idx_t result_offset, ScanVectorType scan_type);
	//! Scan a subset of a vector (defined by the selection vector)
	void Select(ColumnScanState &state, idx_t scan_count, Vector &result, const SelectionVector &sel, idx_t sel_count);
	//! Scan one vector while applying a filter to the vector, returning only the matching elements
	void Filter(ColumnScanState &state, idx_t scan_count, Vector &result, SelectionVector &sel, idx_t &sel_count,
	            const TableFilter &filter, TableFilterState &filter_state);
	//! Fetch a value of the specific row id and append it to the result
	void FetchRow(ColumnFetchState &state, row_t row_id, Vector &result, idx_t result_idx);

	static idx_t FilterSelection(SelectionVector &sel, Vector &vector, UnifiedVectorFormat &vdata,
	                             const TableFilter &filter, TableFilterState &filter_state, idx_t scan_count,
	                             idx_t &approved_tuple_count);

	//! Skip a scan forward to the row_index specified in the scan state
	void Skip(ColumnScanState &state);

	// The maximum size of the buffer (in bytes)
	idx_t SegmentSize() const;
	//! Resize the block
	void Resize(idx_t segment_size);
	const CompressionFunction &GetCompressionFunction();

	//! Initialize an append of this segment. Appends are only supported on transient segments.
	void InitializeAppend(ColumnAppendState &state);
	//! Appends a (part of) vector to the segment, returns the amount of entries successfully appended
	idx_t Append(ColumnAppendState &state, UnifiedVectorFormat &data, idx_t offset, idx_t count);
	//! Finalize the segment for appending - no more appends can follow on this segment
	//! The segment should be compacted as much as possible
	//! Returns the number of bytes occupied within the segment
	idx_t FinalizeAppend(ColumnAppendState &state);
	//! Revert an append made to this segment
	void RevertAppend(idx_t new_count);

	//! Convert a transient in-memory segment to a persistent segment backed by an on-disk block.
	//! Only used during checkpointing.
	void ConvertToPersistent(QueryContext context, optional_ptr<BlockManager> block_manager, const block_id_t block_id);
	//! Updates pointers to refer to the given block and offset. This is only used
	//! when sharing a block among segments. This is invoked only AFTER the block is written.
	void MarkAsPersistent(shared_ptr<BlockHandle> block, uint32_t offset_in_block);
	void SetBlock(shared_ptr<BlockHandle> block, uint32_t offset);
	//! Gets a data pointer from a persistent column segment
	DataPointer GetDataPointer(idx_t row_start);

	block_id_t GetBlockId() {
		D_ASSERT(segment_type == ColumnSegmentType::PERSISTENT);
		return block_id;
	}

	//! Returns the block manager handling this segment. For transient segments, this might be the temporary block
	//! manager. Later, we possibly convert this (transient) segment to a persistent segment. In that case, there
	//! exists another block manager handling the ColumnData, of which this segment is a part.
	BlockManager &GetBlockManager() const {
		return block->block_manager;
	}

	idx_t GetBlockOffset() {
		D_ASSERT(segment_type == ColumnSegmentType::PERSISTENT || offset == 0);
		return offset;
	}

	optional_ptr<CompressedSegmentState> GetSegmentState() const {
		return segment_state.get();
	}

	void VisitBlockIds(BlockIdVisitor &visitor) const;

private:
	void Scan(ColumnScanState &state, idx_t scan_count, Vector &result);
	void ScanPartial(ColumnScanState &state, idx_t scan_count, Vector &result, idx_t result_offset);

public:
	//! The database instance
	DatabaseInstance &db;
	//! The type stored in the column
	LogicalType type;
	//! The size of the type
	idx_t type_size;
	//! The column segment type (transient or persistent)
	ColumnSegmentType segment_type;
	//! The statistics for the segment
	SegmentStatistics stats;
	//! The block that this segment relates to
	shared_ptr<BlockHandle> block;

private:
	//! The compression function
	reference<CompressionFunction> function;
	//! The block id that this segment relates to (persistent segment only)
	block_id_t block_id;
	//! The offset into the block (persistent segment only)
	idx_t offset;
	//! The allocated segment size, which is bounded by Storage::BLOCK_SIZE
	idx_t segment_size;
	//! Storage associated with the compressed segment
	unique_ptr<CompressedSegmentState> segment_state;
};

} // namespace goose
