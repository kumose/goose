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

#include <goose/storage/checkpoint_manager.h>
#include <goose/common/serializer/memory_stream.h>

namespace goose {
struct ColumnCheckpointState;
class CheckpointWriter;
class ColumnData;
class ColumnSegment;
class RowGroup;
class BaseStatistics;
class SegmentStatistics;

// Writes data for an entire row group.
class RowGroupWriter {
public:
	RowGroupWriter(TableCatalogEntry &table, PartialBlockManager &partial_block_manager);
	virtual ~RowGroupWriter() {
	}

	const vector<CompressionType> &GetCompressionTypes() const {
		return compression_types;
	}

	virtual CheckpointOptions GetCheckpointOptions() const = 0;
	virtual WriteStream &GetPayloadWriter() = 0;
	virtual MetaBlockPointer GetMetaBlockPointer() = 0;
	virtual optional_ptr<MetadataManager> GetMetadataManager() = 0;
	virtual void StartWritingColumns(vector<MetaBlockPointer> &column_metadata) {
	}
	virtual void FinishWritingColumns() {
	}

	DatabaseInstance &GetDatabase();
	PartialBlockManager &GetPartialBlockManager() {
		return partial_block_manager;
	}

protected:
	TableCatalogEntry &table;
	PartialBlockManager &partial_block_manager;
	vector<CompressionType> compression_types;
};

// Writes data for an entire row group.
class SingleFileRowGroupWriter : public RowGroupWriter {
public:
	SingleFileRowGroupWriter(TableCatalogEntry &table, PartialBlockManager &partial_block_manager,
	                         TableDataWriter &writer, MetadataWriter &table_data_writer);

public:
	CheckpointOptions GetCheckpointOptions() const override;
	WriteStream &GetPayloadWriter() override;
	MetaBlockPointer GetMetaBlockPointer() override;
	optional_ptr<MetadataManager> GetMetadataManager() override;
	void StartWritingColumns(vector<MetaBlockPointer> &column_metadata) override;
	void FinishWritingColumns() override;

private:
	//! Underlying writer object
	TableDataWriter &writer;
	//! MetadataWriter is a cursor on a given BlockManager. This returns the
	//! cursor against which we should write payload data for the specified RowGroup.
	MetadataWriter &table_data_writer;
};

} // namespace goose
