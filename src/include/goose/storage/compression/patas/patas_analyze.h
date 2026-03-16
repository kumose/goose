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

#include <goose/storage/compression/patas/patas.h>
#include <goose/function/compression_function.h>

namespace goose {

struct EmptyPatasWriter;

template <class T>
struct PatasAnalyzeState : public AnalyzeState {};

template <class T>
unique_ptr<AnalyzeState> PatasInitAnalyze(ColumnData &col_data, PhysicalType type) {
	// This compression type is deprecated
	return nullptr;
}

template <class T>
bool PatasAnalyze(AnalyzeState &state, Vector &input, idx_t count) {
	throw InternalException("Patas has been deprecated, can no longer be used to compress data");
	return false;
}

template <class T>
idx_t PatasFinalAnalyze(AnalyzeState &state) {
	throw InternalException("Patas has been deprecated, can no longer be used to compress data");
}

} // namespace goose
