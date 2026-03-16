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

#include <goose/function/table_function.h>
#include <goose/function/built_in_functions.h>

namespace goose {

struct CheckpointFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GlobTableFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct RangeTableFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct RepeatTableFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct RepeatRowTableFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct UnnestTableFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct CSVSnifferFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct ReadBlobFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct ReadTextFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct QueryTableFunction {
	static void RegisterFunction(BuiltinFunctions &set);
};

} // namespace goose
