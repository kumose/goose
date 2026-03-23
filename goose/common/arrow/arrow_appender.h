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

#include <goose/common/arrow/arrow_converter.h>
#include <goose/common/arrow/arrow.h>

namespace goose {

struct ArrowAppendData;

class ArrowTypeExtensionData;

//! The ArrowAppender class can be used to incrementally construct an arrow array by appending data chunks into it
class ArrowAppender {
public:
	GOOSE_API ArrowAppender(vector<LogicalType> types_p, const idx_t initial_capacity, ClientProperties options,
	                         unordered_map<idx_t, const shared_ptr<ArrowTypeExtensionData>> extension_type_cast);
	GOOSE_API ~ArrowAppender();

public:
	//! Append a data chunk to the underlying arrow array
	GOOSE_API void Append(DataChunk &input, idx_t from, idx_t to, idx_t input_size);
	//! Returns the underlying arrow array
	GOOSE_API ArrowArray Finalize();
	idx_t RowCount() const;
	static void ReleaseArray(ArrowArray *array);
	static ArrowArray *FinalizeChild(const LogicalType &type, unique_ptr<ArrowAppendData> append_data_p);
	static unique_ptr<ArrowAppendData>
	InitializeChild(const LogicalType &type, const idx_t capacity, ClientProperties &options,
	                const shared_ptr<ArrowTypeExtensionData> &extension_type = nullptr);
	static void AddChildren(ArrowAppendData &data, const idx_t count);

private:
	//! The types of the chunks that will be appended in
	vector<LogicalType> types;
	//! The root arrow append data
	vector<unique_ptr<ArrowAppendData>> root_data;
	//! The total row count that has been appended
	idx_t row_count = 0;

	ClientProperties options;
};

} // namespace goose
