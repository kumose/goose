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

#include "writer/primitive_column_writer.h"

namespace goose {

class BooleanColumnWriter : public PrimitiveColumnWriter {
public:
	BooleanColumnWriter(ParquetWriter &writer, ParquetColumnSchema &&column_schema, vector<string> schema_path_p);
	~BooleanColumnWriter() override = default;

public:
	unique_ptr<ColumnWriterStatistics> InitializeStatsState() override;

	void WriteVector(WriteStream &temp_writer, ColumnWriterStatistics *stats_p, ColumnWriterPageState *state_p,
	                 Vector &input_column, idx_t chunk_start, idx_t chunk_end) override;

	unique_ptr<ColumnWriterPageState> InitializePageState(PrimitiveColumnWriterState &state, idx_t page_idx) override;
	void FlushPageState(WriteStream &temp_writer, ColumnWriterPageState *state_p) override;

	idx_t GetRowSize(const Vector &vector, const idx_t index, const PrimitiveColumnWriterState &state) const override;
};

} // namespace goose
