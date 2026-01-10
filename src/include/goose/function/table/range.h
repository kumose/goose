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
