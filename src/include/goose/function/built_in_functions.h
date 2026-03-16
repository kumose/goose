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

#include <goose/function/function.h>
#include <goose/catalog/catalog_transaction.h>

namespace goose {

class BuiltinFunctions {
public:
	BuiltinFunctions(CatalogTransaction transaction, Catalog &catalog);
	~BuiltinFunctions();

	//! Initialize a catalog with all built-in functions
	void Initialize();

public:
	void AddFunction(AggregateFunctionSet set);
	void AddFunction(AggregateFunction function);
	void AddFunction(ScalarFunctionSet set);
	void AddFunction(PragmaFunction function);
	void AddFunction(const string &name, PragmaFunctionSet functions);
	void AddFunction(ScalarFunction function);
	void AddFunction(const vector<string> &names, ScalarFunction function);
	void AddFunction(TableFunctionSet set);
	void AddFunction(TableFunction function);
	void AddFunction(CopyFunction function);

	void AddCollation(string name, ScalarFunction function, bool combinable = false,
	                  bool not_required_for_equality = false);

private:
	CatalogTransaction transaction;
	Catalog &catalog;

private:
	template <class T>
	void Register() {
		T::RegisterFunction(*this);
	}

	// table-producing functions
	void RegisterTableScanFunctions();
	void RegisterSQLiteFunctions();
	void RegisterReadFunctions();
	void RegisterTableFunctions();
	void RegisterArrowFunctions();
	void RegisterSnifferFunction();
	void RegisterCopyFunctions();

	void RegisterExtensionOverloads();

	// pragmas
	void RegisterPragmaFunctions();

	void AddExtensionFunction(ScalarFunctionSet set);
};

} // namespace goose
