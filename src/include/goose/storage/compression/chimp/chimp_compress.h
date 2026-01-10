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

#include <goose/storage/compression/chimp/chimp.h>
#include <goose/function/compression_function.h>
#include <goose/storage/compression/chimp/chimp_analyze.h>

#include <goose/common/helper.h>
#include <goose/common/limits.h>
#include <goose/common/types/null_value.h>
#include <goose/function/compression/compression.h>
#include <goose/main/config.h>
#include <goose/storage/buffer_manager.h>

#include <goose/storage/table/column_data_checkpointer.h>
#include <goose/storage/table/column_segment.h>
#include <goose/common/operator/subtract.h>

#include <functional>

namespace goose {

template <class T>
struct ChimpCompressionState : public CompressionState {};

// Compression Functions

template <class T>
unique_ptr<CompressionState> ChimpInitCompression(ColumnDataCheckpointData &checkpoint_data,
                                                  unique_ptr<AnalyzeState> state) {
	throw InternalException("Chimp has been deprecated, can no longer be used to compress data");
	return nullptr;
}

template <class T>
void ChimpCompress(CompressionState &state_p, Vector &scan_vector, idx_t count) {
	throw InternalException("Chimp has been deprecated, can no longer be used to compress data");
}

template <class T>
void ChimpFinalizeCompress(CompressionState &state_p) {
	throw InternalException("Chimp has been deprecated, can no longer be used to compress data");
}

} // namespace goose
