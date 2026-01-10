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

#include "column_writer.h"
#include "writer/parquet_write_stats.h"
#include <goose/common/serializer/memory_stream.h>
#include "parquet_statistics.h"

namespace goose {

struct PageInformation {
	idx_t offset = 0;
	idx_t row_count = 0;
	idx_t empty_count = 0;
	idx_t estimated_page_size = 0;
	idx_t null_count = 0;
};

struct PageWriteInformation {
	goose_parquet::PageHeader page_header;
	unique_ptr<MemoryStream> temp_writer;
	unique_ptr<ColumnWriterPageState> page_state;
	idx_t write_page_idx = 0;
	idx_t write_count = 0;
	idx_t max_write_count = 0;
	size_t compressed_size;
	data_ptr_t compressed_data;
	AllocatedData compressed_buf;
};

class PrimitiveColumnWriterState : public ColumnWriterState {
public:
	PrimitiveColumnWriterState(ParquetWriter &writer_p, goose_parquet::RowGroup &row_group, idx_t col_idx)
	    : writer(writer_p), row_group(row_group), col_idx(col_idx) {
		page_info.emplace_back();
	}
	~PrimitiveColumnWriterState() override = default;

	ParquetWriter &writer;
	goose_parquet::RowGroup &row_group;
	idx_t col_idx;
	vector<PageInformation> page_info;
	vector<PageWriteInformation> write_info;
	unique_ptr<ColumnWriterStatistics> stats_state;
	idx_t current_page = 0;

	unique_ptr<ParquetBloomFilter> bloom_filter;
};

//! Base class for writing non-compound types (ex. numerics, strings)
class PrimitiveColumnWriter : public ColumnWriter {
public:
	PrimitiveColumnWriter(ParquetWriter &writer, ParquetColumnSchema &&column_schema, vector<string> schema_path);
	~PrimitiveColumnWriter() override = default;

	//! We limit the uncompressed page size to 100MB
	//! The max size in Parquet is 2GB, but we choose a more conservative limit
	static constexpr const idx_t MAX_UNCOMPRESSED_PAGE_SIZE = 104857600ULL;
	//! Dictionary pages must be below 2GB. Unlike data pages, there's only one dictionary page.
	//! For this reason we go with a much higher, but still a conservative upper bound of 1GB;
	static constexpr const idx_t MAX_UNCOMPRESSED_DICT_PAGE_SIZE = 1073741824ULL;

public:
	unique_ptr<ColumnWriterState> InitializeWriteState(goose_parquet::RowGroup &row_group) override;
	void Prepare(ColumnWriterState &state, ColumnWriterState *parent, Vector &vector, idx_t count,
	             bool vector_can_span_multiple_pages) override;
	void BeginWrite(ColumnWriterState &state) override;
	void Write(ColumnWriterState &state, Vector &vector, idx_t count) override;
	void FinalizeWrite(ColumnWriterState &state) override;
	void FinalizeSchema(vector<goose_parquet::SchemaElement> &schemas) override;

protected:
	static void WriteLevels(Allocator &allocator, WriteStream &temp_writer, const unsafe_vector<uint16_t> &levels,
	                        idx_t max_value, idx_t start_offset, idx_t count, optional_idx null_count = optional_idx());

	virtual goose_parquet::Encoding::type GetEncoding(PrimitiveColumnWriterState &state);

	void NextPage(PrimitiveColumnWriterState &state);
	void FlushPage(PrimitiveColumnWriterState &state);

	//! Initializes the state used to track statistics during writing. Only used for scalar types.
	virtual unique_ptr<ColumnWriterStatistics> InitializeStatsState();

	//! Initialize the writer for a specific page. Only used for scalar types.
	virtual unique_ptr<ColumnWriterPageState> InitializePageState(PrimitiveColumnWriterState &state, idx_t page_idx);

	//! Flushes the writer for a specific page. Only used for scalar types.
	virtual void FlushPageState(WriteStream &temp_writer, ColumnWriterPageState *state);

	//! Retrieves the row size of a vector at the specified location. Only used for scalar types.
	virtual idx_t GetRowSize(const Vector &vector, const idx_t index, const PrimitiveColumnWriterState &state) const;
	//! Writes a (subset of a) vector to the specified serializer. Only used for scalar types.
	virtual void WriteVector(WriteStream &temp_writer, ColumnWriterStatistics *stats, ColumnWriterPageState *page_state,
	                         Vector &vector, idx_t chunk_start, idx_t chunk_end) = 0;

	virtual bool HasDictionary(PrimitiveColumnWriterState &state_p) {
		return false;
	}
	//! The number of elements in the dictionary
	virtual idx_t DictionarySize(PrimitiveColumnWriterState &state_p);
	void WriteDictionary(PrimitiveColumnWriterState &state, unique_ptr<MemoryStream> temp_writer, idx_t row_count);
	virtual void FlushDictionary(PrimitiveColumnWriterState &state, ColumnWriterStatistics *stats);

	void SetParquetStatistics(PrimitiveColumnWriterState &state, goose_parquet::ColumnChunk &column);
	void RegisterToRowGroup(goose_parquet::RowGroup &row_group);
};

} // namespace goose
