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
