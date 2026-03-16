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

#include "writer/primitive_column_writer.h"

namespace goose {
class EnumWriterPageState;

class EnumColumnWriter : public PrimitiveColumnWriter {
public:
	EnumColumnWriter(ParquetWriter &writer, ParquetColumnSchema &&column_schema, vector<string> schema_path_p);
	~EnumColumnWriter() override = default;

	uint32_t bit_width;

public:
	unique_ptr<ColumnWriterStatistics> InitializeStatsState() override;

	void WriteVector(WriteStream &temp_writer, ColumnWriterStatistics *stats_p, ColumnWriterPageState *page_state_p,
	                 Vector &input_column, idx_t chunk_start, idx_t chunk_end) override;

	unique_ptr<ColumnWriterPageState> InitializePageState(PrimitiveColumnWriterState &state, idx_t page_idx) override;

	void FlushPageState(WriteStream &temp_writer, ColumnWriterPageState *state_p) override;

	goose_parquet::Encoding::type GetEncoding(PrimitiveColumnWriterState &state) override;

	bool HasDictionary(PrimitiveColumnWriterState &state) override;

	idx_t DictionarySize(PrimitiveColumnWriterState &state_p) override;

	void FlushDictionary(PrimitiveColumnWriterState &state, ColumnWriterStatistics *stats_p) override;

	idx_t GetRowSize(const Vector &vector, const idx_t index, const PrimitiveColumnWriterState &state) const override;

private:
	template <class T>
	void WriteEnumInternal(WriteStream &temp_writer, Vector &input_column, idx_t chunk_start, idx_t chunk_end,
	                       EnumWriterPageState &page_state);
};

} // namespace goose
