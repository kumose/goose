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

#include <goose/extension/parquet/writer/list_column_writer.h>

namespace goose {

class ArrayColumnWriter : public ListColumnWriter {
public:
	ArrayColumnWriter(ParquetWriter &writer, ParquetColumnSchema &&column_schema, vector<string> schema_path_p,
	                  unique_ptr<ColumnWriter> child_writer_p)
	    : ListColumnWriter(writer, std::move(column_schema), std::move(schema_path_p), std::move(child_writer_p)) {
	}
	~ArrayColumnWriter() override = default;

public:
	void Analyze(ColumnWriterState &state, ColumnWriterState *parent, Vector &vector, idx_t count) override;
	void Prepare(ColumnWriterState &state, ColumnWriterState *parent, Vector &vector, idx_t count,
	             bool vector_can_span_multiple_pages) override;
	void Write(ColumnWriterState &state, Vector &vector, idx_t count) override;

protected:
	void WriteArrayState(ListColumnWriterState &state, idx_t array_size, uint16_t first_repeat_level,
	                     idx_t define_value, const bool is_empty = false);
};

} // namespace goose
