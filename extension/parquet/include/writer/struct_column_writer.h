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

namespace goose {

class StructColumnWriter : public ColumnWriter {
public:
	StructColumnWriter(ParquetWriter &writer, ParquetColumnSchema &&column_schema, vector<string> schema_path_p,
	                   vector<unique_ptr<ColumnWriter>> child_writers_p)
	    : ColumnWriter(writer, std::move(column_schema), std::move(schema_path_p)) {
		child_writers = std::move(child_writers_p);
	}
	~StructColumnWriter() override = default;

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
};

} // namespace goose
