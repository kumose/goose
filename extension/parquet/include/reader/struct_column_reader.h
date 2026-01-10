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

#include "column_reader.h"
#include "reader/templated_column_reader.h"

namespace goose {

class StructColumnReader : public ColumnReader {
public:
	static constexpr const PhysicalType TYPE = PhysicalType::STRUCT;

public:
	StructColumnReader(ParquetReader &reader, const ParquetColumnSchema &schema,
	                   vector<unique_ptr<ColumnReader>> child_readers_p);

	vector<unique_ptr<ColumnReader>> child_readers;

public:
	ColumnReader &GetChildReader(idx_t child_idx);

	void InitializeRead(idx_t row_group_idx_p, const vector<ColumnChunk> &columns, TProtocol &protocol_p) override;

	idx_t Read(uint64_t num_values, data_ptr_t define_out, data_ptr_t repeat_out, Vector &result) override;

	void Skip(idx_t num_values) override;
	idx_t GroupRowsAvailable() override;
	uint64_t TotalCompressedSize() override;
	void RegisterPrefetch(ThriftFileTransport &transport, bool allow_merge) override;
};

} // namespace goose
