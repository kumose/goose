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
