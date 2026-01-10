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

#include <goose/common/vector_size.h>
#include <goose/storage/table/chunk_info.h>
#include <goose/storage/storage_info.h>
#include <goose/common/types-import.h>
#include <goose/execution/index/fixed_size_allocator.h>

namespace goose {

struct DeleteInfo;
class MetadataManager;
class BufferManager;
struct MetaBlockPointer;

class RowVersionManager {
public:
	explicit RowVersionManager(BufferManager &buffer_manager) noexcept;

	idx_t GetCommittedDeletedCount(idx_t count);

	bool ShouldCheckpointRowGroup(transaction_t checkpoint_id, idx_t count);
	idx_t GetSelVector(TransactionData transaction, idx_t vector_idx, SelectionVector &sel_vector, idx_t max_count);
	idx_t GetCommittedSelVector(transaction_t start_time, transaction_t transaction_id, idx_t vector_idx,
	                            SelectionVector &sel_vector, idx_t max_count);
	bool Fetch(TransactionData transaction, idx_t row);

	void AppendVersionInfo(TransactionData transaction, idx_t count, idx_t row_group_start, idx_t row_group_end);
	void CommitAppend(transaction_t commit_id, idx_t row_group_start, idx_t count);
	void RevertAppend(idx_t new_count);
	void CleanupAppend(transaction_t lowest_active_transaction, idx_t row_group_start, idx_t count);

	idx_t DeleteRows(idx_t vector_idx, transaction_t transaction_id, row_t rows[], idx_t count);
	void CommitDelete(idx_t vector_idx, transaction_t commit_id, const DeleteInfo &info);

	vector<MetaBlockPointer> Checkpoint(RowGroupWriter &writer);
	static shared_ptr<RowVersionManager> Deserialize(MetaBlockPointer delete_pointer, MetadataManager &manager);

	bool HasUnserializedChanges();
	vector<MetaBlockPointer> GetStoragePointers();

private:
	mutex version_lock;
	FixedSizeAllocator allocator;
	vector<unique_ptr<ChunkInfo>> vector_info;
	optional_idx uncheckpointed_delete_commit;
	vector<MetaBlockPointer> storage_pointers;

private:
	FixedSizeAllocator &GetAllocator() {
		return allocator;
	}
	optional_ptr<ChunkInfo> GetChunkInfo(idx_t vector_idx);
	ChunkVectorInfo &GetVectorInfo(idx_t vector_idx);
	void FillVectorInfo(idx_t vector_idx);
};

} // namespace goose
