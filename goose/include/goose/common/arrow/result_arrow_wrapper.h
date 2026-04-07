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

#include <goose/main/query_result.h>
#include <goose/common/arrow/arrow_wrapper.h>
#include <goose/main/chunk_scan_state.h>
#include <goose/function/table/arrow/arrow_goose_schema.h>

namespace goose {
class ResultArrowArrayStreamWrapper {
public:
	explicit ResultArrowArrayStreamWrapper(unique_ptr<QueryResult> result, idx_t batch_size);

public:
	ArrowArrayStream stream;
	unique_ptr<QueryResult> result;
	ErrorData last_error;
	idx_t batch_size;
	vector<LogicalType> column_types;
	vector<string> column_names;
	unique_ptr<ChunkScanState> scan_state;
	unordered_map<idx_t, const shared_ptr<ArrowTypeExtensionData>> extension_types;

private:
	static int MyStreamGetSchema(struct ArrowArrayStream *stream, struct ArrowSchema *out);
	static int MyStreamGetNext(struct ArrowArrayStream *stream, struct ArrowArray *out);
	static void MyStreamRelease(struct ArrowArrayStream *stream);
	static const char *MyStreamGetLastError(struct ArrowArrayStream *stream);
};
} // namespace goose
