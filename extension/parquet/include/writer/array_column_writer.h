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

#include "writer/list_column_writer.h"

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
