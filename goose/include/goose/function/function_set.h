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

#include <goose/function/aggregate_function.h>
#include <goose/function/scalar_function.h>
#include <goose/function/table_function.h>
#include <goose/function/pragma_function.h>

namespace goose {

template <class T>
class FunctionSet {
public:
	explicit FunctionSet(string name) : name(std::move(name)) {
	}

	//! The name of the function set
	string name;
	//! The set of functions.
	vector<T> functions;

public:
	void AddFunction(T function) {
		functions.push_back(std::move(function));
	}
	idx_t Size() {
		return functions.size();
	}
	T GetFunctionByOffset(idx_t offset) {
		D_ASSERT(offset < functions.size());
		return functions[offset];
	}
	T &GetFunctionReferenceByOffset(idx_t offset) {
		D_ASSERT(offset < functions.size());
		return functions[offset];
	}
	bool MergeFunctionSet(FunctionSet<T> new_functions, bool override = false) {
		D_ASSERT(!new_functions.functions.empty());
		for (auto &new_func : new_functions.functions) {
			bool overwritten = false;
			for (auto &func : functions) {
				if (new_func.Equal(func)) {
					// function overload already exists
					if (override) {
						// override it
						overwritten = true;
						func = new_func;
					} else {
						// throw an error
						return false;
					}
					break;
				}
			}
			if (!overwritten) {
				functions.push_back(new_func);
			}
		}
		return true;
	}
};

class ScalarFunctionSet : public FunctionSet<ScalarFunction> {
public:
	GOOSE_API explicit ScalarFunctionSet();
	GOOSE_API explicit ScalarFunctionSet(string name);
	GOOSE_API explicit ScalarFunctionSet(ScalarFunction fun);

	GOOSE_API ScalarFunction GetFunctionByArguments(ClientContext &context, const vector<LogicalType> &arguments);
};

class AggregateFunctionSet : public FunctionSet<AggregateFunction> {
public:
	GOOSE_API explicit AggregateFunctionSet();
	GOOSE_API explicit AggregateFunctionSet(string name);
	GOOSE_API explicit AggregateFunctionSet(AggregateFunction fun);

	GOOSE_API AggregateFunction GetFunctionByArguments(ClientContext &context, const vector<LogicalType> &arguments);
};

class TableFunctionSet : public FunctionSet<TableFunction> {
public:
	GOOSE_API explicit TableFunctionSet(string name);
	GOOSE_API explicit TableFunctionSet(TableFunction fun);

	TableFunction GetFunctionByArguments(ClientContext &context, const vector<LogicalType> &arguments);
};

class PragmaFunctionSet : public FunctionSet<PragmaFunction> {
public:
	GOOSE_API explicit PragmaFunctionSet(string name);
	GOOSE_API explicit PragmaFunctionSet(PragmaFunction fun);
};

} // namespace goose
