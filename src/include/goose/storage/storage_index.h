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

#include <goose/common/constants.h>
#include <goose/common/vector.h>
#include <goose/common/types.h>
#include <goose/common/column_index.h>

namespace goose {

enum class StorageIndexType : uint8_t { FULL_READ, PUSHDOWN_EXTRACT };

struct StorageIndex {
public:
	StorageIndex() : index(COLUMN_IDENTIFIER_ROW_ID), index_type(StorageIndexType::FULL_READ) {
	}
	explicit StorageIndex(idx_t index) : index(index), index_type(StorageIndexType::FULL_READ) {
	}
	StorageIndex(idx_t index, vector<StorageIndex> child_indexes_p)
	    : index(index), index_type(StorageIndexType::FULL_READ), child_indexes(std::move(child_indexes_p)) {
	}

	inline bool operator==(const StorageIndex &rhs) const {
		return index == rhs.index;
	}
	inline bool operator!=(const StorageIndex &rhs) const {
		return index != rhs.index;
	}
	inline bool operator<(const StorageIndex &rhs) const {
		return index < rhs.index;
	}

public:
	static StorageIndex FromColumnIndex(const ColumnIndex &column_id) {
		vector<StorageIndex> result;
		for (auto &child_id : column_id.GetChildIndexes()) {
			result.push_back(StorageIndex::FromColumnIndex(child_id));
		}
		auto storage_index = StorageIndex(column_id.GetPrimaryIndex(), std::move(result));
		if (column_id.HasType()) {
			storage_index.SetType(column_id.GetType());
		}
		if (column_id.IsPushdownExtract()) {
			storage_index.SetPushdownExtract();
		}
		return storage_index;
	}

public:
	idx_t GetPrimaryIndex() const {
		return index;
	}
	PhysicalIndex ToPhysical() const {
		return PhysicalIndex(index);
	}
	bool HasType() const {
		return type.id() != LogicalTypeId::INVALID;
	}
	const LogicalType &GetType() const {
		return type;
	}
	bool HasChildren() const {
		return !child_indexes.empty();
	}
	idx_t ChildIndexCount() const {
		return child_indexes.size();
	}
	const StorageIndex &GetChildIndex(idx_t idx) const {
		return child_indexes[idx];
	}
	StorageIndex &GetChildIndex(idx_t idx) {
		return child_indexes[idx];
	}
	const vector<StorageIndex> &GetChildIndexes() const {
		return child_indexes;
	}
	void AddChildIndex(StorageIndex new_index) {
		this->child_indexes.push_back(std::move(new_index));
	}
	void SetType(const LogicalType &type_information) {
		type = type_information;
	}
	void SetPushdownExtract() {
		D_ASSERT(!IsPushdownExtract());
		index_type = StorageIndexType::PUSHDOWN_EXTRACT;
	}
	bool IsPushdownExtract() const {
		return index_type == StorageIndexType::PUSHDOWN_EXTRACT;
	}
	void SetIndex(idx_t new_index) {
		index = new_index;
	}
	bool IsRowIdColumn() const {
		return index == DConstants::INVALID_INDEX;
	}

private:
	idx_t index;
	LogicalType type = LogicalType::INVALID;
	StorageIndexType index_type;
	vector<StorageIndex> child_indexes;
};

} // namespace goose
