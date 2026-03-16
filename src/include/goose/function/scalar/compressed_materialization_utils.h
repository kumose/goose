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
