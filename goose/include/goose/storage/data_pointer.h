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

#include <goose/common/common.h>
#include <goose/storage/statistics/base_statistics.h>
#include <goose/storage/storage_info.h>
#include <goose/storage/block.h>
#include <goose/storage/table/row_group.h>
#include <goose/common/enums/compression_type.h>

namespace goose {

class Serializer;
class Deserializer;
class QueryContext;

struct ColumnSegmentState {
	virtual ~ColumnSegmentState() {
	}

	virtual void Serialize(Serializer &serializer) const = 0;
	static unique_ptr<ColumnSegmentState> Deserialize(Deserializer &deserializer);

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

public:
	vector<block_id_t> blocks;
};

struct DataPointer {
	explicit DataPointer(BaseStatistics stats);
	// disable copy constructors
	DataPointer(const DataPointer &other) = delete;
	DataPointer &operator=(const DataPointer &) = delete;
	//! enable move constructors
	GOOSE_API DataPointer(DataPointer &&other) noexcept;
	GOOSE_API DataPointer &operator=(DataPointer &&) noexcept;

	uint64_t row_start;
	uint64_t tuple_count;
	BlockPointer block_pointer;
	CompressionType compression_type;
	//! Type-specific statistics of the segment
	BaseStatistics statistics;
	//! Serialized segment state
	unique_ptr<ColumnSegmentState> segment_state;

	void Serialize(Serializer &serializer) const;
	static DataPointer Deserialize(Deserializer &source);
};

struct RowGroupPointer {
	uint64_t row_start;
	uint64_t tuple_count;
	//! The data pointers of the column segments stored in the row group
	vector<MetaBlockPointer> data_pointers;
	//! Data pointers to the delete information of the row group (if any)
	vector<MetaBlockPointer> deletes_pointers;
	//! Whether or not we have all metadata blocks defined in the pointer
	bool has_metadata_blocks = false;
	//! Metadata blocks of the columns that are not mentioned in "data_pointers"
	//! This is often empty - but can be set for wide columns with a lot of metadata
	vector<idx_t> extra_metadata_blocks;
};

} // namespace goose
