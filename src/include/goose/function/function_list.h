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

#include <goose/goose.h>

namespace goose {

typedef ScalarFunction (*get_scalar_function_t)();
typedef ScalarFunctionSet (*get_scalar_function_set_t)();
typedef AggregateFunction (*get_aggregate_function_t)();
typedef AggregateFunctionSet (*get_aggregate_function_set_t)();

struct StaticFunctionDefinition {
	const char *name;
	const char *alias_of;
	const char *parameters;
	const char *description;
	const char *example;
	const char *categories;
	get_scalar_function_t get_function;
	get_scalar_function_set_t get_function_set;
	get_aggregate_function_t get_aggregate_function;
	get_aggregate_function_set_t get_aggregate_function_set;
};

class Catalog;
struct CatalogTransaction;

struct FunctionList {
	static const StaticFunctionDefinition *GetInternalFunctionList();
	static void RegisterFunctions(Catalog &catalog, CatalogTransaction transaction);
	static void RegisterExtensionFunctions(ExtensionLoader &db, const StaticFunctionDefinition *functions);
};

} // namespace goose
