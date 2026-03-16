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

#include <goose/storage/compression/chimp/chimp.h>
#include <goose/function/compression_function.h>

namespace goose {

struct EmptyChimpWriter;

template <class T>
struct ChimpAnalyzeState : public AnalyzeState {};

template <class T>
unique_ptr<AnalyzeState> ChimpInitAnalyze(ColumnData &col_data, PhysicalType type) {
	// This compression type is deprecated
	return nullptr;
}

template <class T>
bool ChimpAnalyze(AnalyzeState &state, Vector &input, idx_t count) {
	throw InternalException("Chimp has been deprecated, can no longer be used to compress data");
	return false;
}

template <class T>
idx_t ChimpFinalAnalyze(AnalyzeState &state) {
	throw InternalException("Chimp has been deprecated, can no longer be used to compress data");
}

} // namespace goose
