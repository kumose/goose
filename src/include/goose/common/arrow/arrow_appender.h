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
