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

#include <goose/storage/table/column_segment.h>

namespace goose {
class DatabaseInstance;

struct UncompressedFunctions {
	static unique_ptr<CompressionState> InitCompression(ColumnDataCheckpointData &checkpoint_data,
	                                                    unique_ptr<AnalyzeState> state);
	static void Compress(CompressionState &state_p, Vector &data, idx_t count);
	static void FinalizeCompress(CompressionState &state_p);
	static void EmptySkip(ColumnSegment &segment, ColumnScanState &state, idx_t skip_count) {
	}
};

struct FixedSizeUncompressed {
	static CompressionFunction GetFunction(PhysicalType data_type);
};

struct ValidityUncompressed {
public:
	static CompressionFunction GetFunction(PhysicalType data_type);
	static void AlignedScan(data_ptr_t input, idx_t input_start, Vector &result, idx_t scan_count);
	static void UnalignedScan(data_ptr_t input, idx_t input_size, idx_t input_start, Vector &result,
	                          idx_t result_offset, idx_t scan_count);

public:
	static const validity_t LOWER_MASKS[65];
	static const validity_t UPPER_MASKS[65];
};

struct StringUncompressed {
public:
	static CompressionFunction GetFunction(PhysicalType data_type);
	static idx_t GetStringBlockLimit(const idx_t block_size) {
		return MinValue(AlignValueFloor(block_size / 4), DEFAULT_STRING_BLOCK_LIMIT);
	}

public:
	//! The default maximum string size for sufficiently big block sizes
	static constexpr idx_t DEFAULT_STRING_BLOCK_LIMIT = 4096;
};

} // namespace goose
