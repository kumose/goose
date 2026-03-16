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

#include <goose/goose.h>
#include "parquet_dbp_decoder.h"
#include "resizable_buffer.h"

namespace goose {
class ColumnReader;

class DeltaLengthByteArrayDecoder {
public:
	explicit DeltaLengthByteArrayDecoder(ColumnReader &reader);

public:
	void InitializePage();

	void Read(shared_ptr<ResizeableBuffer> &block, uint8_t *defines, idx_t read_count, Vector &result,
	          idx_t result_offset);
	void Skip(uint8_t *defines, idx_t skip_count);

private:
	template <bool HAS_DEFINES, bool VALIDATE_INDIVIDUAL_STRINGS>
	void ReadInternal(shared_ptr<ResizeableBuffer> &block, uint8_t *defines, idx_t read_count, Vector &result,
	                  idx_t result_offset);
	template <bool HAS_DEFINES>
	void SkipInternal(uint8_t *defines, idx_t skip_count);

private:
	ColumnReader &reader;
	ResizeableBuffer &length_buffer;
	idx_t byte_array_count = 0;
	idx_t length_idx;
};

} // namespace goose
