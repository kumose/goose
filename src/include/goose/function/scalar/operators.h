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

#include <goose/function/function_set.h>
#include <goose/function/scalar_function.h>
#include <goose/function/built_in_functions.h>

namespace goose {

struct AddFunction {
	static ScalarFunction GetFunction(const LogicalType &type);
	static ScalarFunction GetFunction(const LogicalType &left_type, const LogicalType &right_type);
};

struct SubtractFunction {
	static ScalarFunction GetFunction(const LogicalType &type);
	static ScalarFunction GetFunction(const LogicalType &left_type, const LogicalType &right_type);
};

} // namespace goose
