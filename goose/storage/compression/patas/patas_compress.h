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

#include <goose/common/bitpacking.h>
#include <goose/storage/compression/patas/patas.h>
#include <goose/function/compression_function.h>
#include <goose/storage/compression/patas/patas_analyze.h>

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

// State

template <class T>
struct PatasCompressionState : public CompressionState {};

// Compression Functions

template <class T>
unique_ptr<CompressionState> PatasInitCompression(ColumnDataCheckpointData &checkpoint_data,
                                                  unique_ptr<AnalyzeState> state) {
	throw InternalException("Patas has been deprecated, can no longer be used to compress data");
	return nullptr;
}

template <class T>
void PatasCompress(CompressionState &state_p, Vector &scan_vector, idx_t count) {
	throw InternalException("Patas has been deprecated, can no longer be used to compress data");
}

template <class T>
void PatasFinalizeCompress(CompressionState &state_p) {
	throw InternalException("Patas has been deprecated, can no longer be used to compress data");
}

} // namespace goose
