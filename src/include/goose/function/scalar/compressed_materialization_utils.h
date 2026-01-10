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

#include <goose/function/built_in_functions.h>
#include <goose/function/function_set.h>

namespace goose {

struct CMUtils {
	//! The types we compress integral types to
	static const vector<LogicalType> IntegralTypes();
	//! The types we compress strings to
	static const vector<LogicalType> StringTypes();

	static unique_ptr<FunctionData> Bind(ClientContext &context, ScalarFunction &bound_function,
	                                     vector<unique_ptr<Expression>> &arguments);
};

//! Needed for (de)serialization without binding
enum class CompressedMaterializationDirection : uint8_t { INVALID = 0, COMPRESS = 1, DECOMPRESS = 2 };

struct CMIntegralCompressFun {
	static ScalarFunction GetFunction(const LogicalType &input_type, const LogicalType &result_type);
};

struct CMIntegralDecompressFun {
	static ScalarFunction GetFunction(const LogicalType &input_type, const LogicalType &result_type);
};

struct CMStringCompressFun {
	static ScalarFunction GetFunction(const LogicalType &result_type);
};

struct CMStringDecompressFun {
	static ScalarFunction GetFunction(const LogicalType &input_type);
};

} // namespace goose
