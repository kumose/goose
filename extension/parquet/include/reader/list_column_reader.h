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

class ListColumnReader : public ColumnReader {
public:
	static constexpr const PhysicalType TYPE = PhysicalType::LIST;

public:
	ListColumnReader(ParquetReader &reader, const ParquetColumnSchema &schema,
	                 unique_ptr<ColumnReader> child_column_reader_p);

	idx_t Read(uint64_t num_values, data_ptr_t define_out, data_ptr_t repeat_out, Vector &result_out) override;

	void ApplyPendingSkips(data_ptr_t define_out, data_ptr_t repeat_out) override;

	void InitializeRead(idx_t row_group_idx_p, const vector<ColumnChunk> &columns, TProtocol &protocol_p) override {
		child_column_reader->InitializeRead(row_group_idx_p, columns, protocol_p);
	}

	idx_t GroupRowsAvailable() override {
		return child_column_reader->GroupRowsAvailable() + overflow_child_count;
	}

	uint64_t TotalCompressedSize() override {
		return child_column_reader->TotalCompressedSize();
	}

	void RegisterPrefetch(ThriftFileTransport &transport, bool allow_merge) override {
		child_column_reader->RegisterPrefetch(transport, allow_merge);
	}

protected:
	template <class OP>
	idx_t ReadInternal(uint64_t num_values, data_ptr_t define_out, data_ptr_t repeat_out,
	                   optional_ptr<Vector> result_out);

private:
	unique_ptr<ColumnReader> child_column_reader;
	ResizeableBuffer child_defines;
	ResizeableBuffer child_repeats;
	uint8_t *child_defines_ptr;
	uint8_t *child_repeats_ptr;

	VectorCache read_cache;
	Vector read_vector;

	idx_t overflow_child_count;
};

} // namespace goose
