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

#include <goose/extension/parquet/column_reader.h>
#include <goose/extension/parquet/reader/templated_column_reader.h>

namespace goose {

class VariantColumnReader : public ColumnReader {
public:
	static constexpr const PhysicalType TYPE = PhysicalType::STRUCT;

public:
	VariantColumnReader(ClientContext &context, ParquetReader &reader, const ParquetColumnSchema &schema,
	                    vector<unique_ptr<ColumnReader>> child_readers_p);

	ClientContext &context;
	vector<unique_ptr<ColumnReader>> child_readers;

public:
	ColumnReader &GetChildReader(idx_t child_idx);

	void InitializeRead(idx_t row_group_idx_p, const vector<ColumnChunk> &columns, TProtocol &protocol_p) override;

	idx_t Read(uint64_t num_values, data_ptr_t define_out, data_ptr_t repeat_out, Vector &result) override;

	void Skip(idx_t num_values) override;
	idx_t GroupRowsAvailable() override;
	uint64_t TotalCompressedSize() override;
	void RegisterPrefetch(ThriftFileTransport &transport, bool allow_merge) override;

protected:
	idx_t metadata_reader_idx;
	idx_t value_reader_idx;
};

} // namespace goose
