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
