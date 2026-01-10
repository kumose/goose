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
