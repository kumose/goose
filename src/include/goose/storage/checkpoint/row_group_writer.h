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
