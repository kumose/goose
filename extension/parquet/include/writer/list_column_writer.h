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

#include "column_writer.h"

namespace goose {

class ListColumnWriterState : public ColumnWriterState {
public:
	ListColumnWriterState(goose_parquet::RowGroup &row_group, idx_t col_idx) : row_group(row_group), col_idx(col_idx) {
	}
	~ListColumnWriterState() override = default;

	goose_parquet::RowGroup &row_group;
	idx_t col_idx;
	unique_ptr<ColumnWriterState> child_state;
	idx_t parent_index = 0;
};

class ListColumnWriter : public ColumnWriter {
public:
	ListColumnWriter(ParquetWriter &writer, ParquetColumnSchema &&column_schema, vector<string> schema_path_p,
	                 unique_ptr<ColumnWriter> child_writer_p)
	    : ColumnWriter(writer, std::move(column_schema), std::move(schema_path_p)) {
		child_writers.push_back(std::move(child_writer_p));
	}
	~ListColumnWriter() override = default;

public:
	unique_ptr<ColumnWriterState> InitializeWriteState(goose_parquet::RowGroup &row_group) override;
	bool HasAnalyze() override;
	void Analyze(ColumnWriterState &state, ColumnWriterState *parent, Vector &vector, idx_t count) override;
	void FinalizeAnalyze(ColumnWriterState &state) override;
	void Prepare(ColumnWriterState &state, ColumnWriterState *parent, Vector &vector, idx_t count,
	             bool vector_can_span_multiple_pages) override;

	void BeginWrite(ColumnWriterState &state) override;
	void Write(ColumnWriterState &state, Vector &vector, idx_t count) override;
	void FinalizeWrite(ColumnWriterState &state) override;
	void FinalizeSchema(vector<goose_parquet::SchemaElement> &schemas) override;

protected:
	ColumnWriter &GetChildWriter();
};

} // namespace goose
