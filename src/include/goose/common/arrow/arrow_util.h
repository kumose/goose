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
#include <goose/common/arrow/arrow.h>
#include <goose/main/chunk_scan_state.h>
#include <goose/main/client_properties.h>
#include <goose/common/helper.h>
#include <goose/common/error_data.h>

namespace goose {

class QueryResult;
class DataChunk;
class ArrowTypeExtensionData;

class ArrowUtil {
public:
	static bool TryFetchChunk(ChunkScanState &scan_state, ClientProperties options, idx_t chunk_size, ArrowArray *out,
	                          idx_t &result_count, ErrorData &error,
	                          unordered_map<idx_t, const shared_ptr<ArrowTypeExtensionData>> extension_type_cast);
	static idx_t FetchChunk(ChunkScanState &scan_state, ClientProperties options, idx_t chunk_size, ArrowArray *out,
	                        const unordered_map<idx_t, const shared_ptr<ArrowTypeExtensionData>> &extension_type_cast);

private:
	static bool TryFetchNext(QueryResult &result, unique_ptr<DataChunk> &out, ErrorData &error);
};

} // namespace goose
