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

#include <goose/common/arrow/appender/append_data.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Unions
//===--------------------------------------------------------------------===//
/**
 * Based on https://arrow.apache.org/docs/format/Columnar.html#union-layout &
 * https://arrow.apache.org/docs/format/CDataInterface.html
 */
struct ArrowUnionData {
public:
	static void Initialize(ArrowAppendData &result, const LogicalType &type, idx_t capacity);
	static void Append(ArrowAppendData &append_data, Vector &input, idx_t from, idx_t to, idx_t input_size);
	static void Finalize(ArrowAppendData &append_data, const LogicalType &type, ArrowArray *result);
};

} // namespace goose
