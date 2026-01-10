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
