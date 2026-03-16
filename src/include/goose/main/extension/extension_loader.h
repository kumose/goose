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

#include <goose/common/constants.h>
#include <goose/function/cast/cast_function_set.h>
#include <goose/function/function_set.h>
#include <goose/main/secret/secret.h>
#include <goose/parser/parsed_data/create_type_info.h>
#include <goose/main/extension_install_info.h>
#include <goose/main/extension_manager.h>

namespace goose {

class DatabaseInstance;
struct CreateMacroInfo;
struct CreateCollationInfo;
struct CreateAggregateFunctionInfo;
struct CreateScalarFunctionInfo;
struct CreateTableFunctionInfo;

class ExtensionLoader {
	friend class Goose;
	friend class ExtensionHelper;

public:
	explicit ExtensionLoader(ExtensionActiveLoad &load_info);
	ExtensionLoader(DatabaseInstance &db, const string &extension_name);

	//! Returns the DatabaseInstance associated with this extension loader
	GOOSE_API DatabaseInstance &GetDatabaseInstance();

public:
	//! Set the description of the extension
	GOOSE_API void SetDescription(const string &description);

public:
	//! Register a new scalar function - merge overloads if the function already exists
	GOOSE_API void RegisterFunction(ScalarFunction function);
	GOOSE_API void RegisterFunction(ScalarFunctionSet function);
	GOOSE_API void RegisterFunction(CreateScalarFunctionInfo info);

	//! Register a new aggregate function - merge overloads if the function already exists
	GOOSE_API void RegisterFunction(AggregateFunction function);
	GOOSE_API void RegisterFunction(AggregateFunctionSet function);
	GOOSE_API void RegisterFunction(CreateAggregateFunctionInfo info);

	//! Register a new table function - merge overloads if the function already exists
	GOOSE_API void RegisterFunction(TableFunction function);
	GOOSE_API void RegisterFunction(TableFunctionSet function);
	GOOSE_API void RegisterFunction(CreateTableFunctionInfo info);

	//! Register a new pragma function - throw an exception if the function already exists
	GOOSE_API void RegisterFunction(PragmaFunction function);

	//! Register a new pragma function set - throw an exception if the function already exists
	GOOSE_API void RegisterFunction(PragmaFunctionSet function);

	//! Register a CreateSecretFunction
	GOOSE_API void RegisterFunction(CreateSecretFunction function);

	//! Register a new copy function - throw an exception if the function already exists
	GOOSE_API void RegisterFunction(CopyFunction function);
	//! Register a new macro function - throw an exception if the function already exists
	GOOSE_API void RegisterFunction(CreateMacroInfo &info);

	//! Register a new collation
	GOOSE_API void RegisterCollation(CreateCollationInfo &info);

	//! Returns a reference to the function in the catalog - throws an exception if it does not exist
	GOOSE_API ScalarFunctionCatalogEntry &GetFunction(const string &name);
	GOOSE_API TableFunctionCatalogEntry &GetTableFunction(const string &name);
	GOOSE_API optional_ptr<CatalogEntry> TryGetFunction(const string &name);
	GOOSE_API optional_ptr<CatalogEntry> TryGetTableFunction(const string &name);

	//! Add a function overload
	GOOSE_API void AddFunctionOverload(ScalarFunction function);
	GOOSE_API void AddFunctionOverload(ScalarFunctionSet function);
	GOOSE_API void AddFunctionOverload(TableFunctionSet function);

	//! Registers a new type
	GOOSE_API void RegisterType(string type_name, LogicalType type,
	                             bind_logical_type_function_t bind_function = nullptr);

	//! Registers a new secret type
	GOOSE_API void RegisterSecretType(SecretType secret_type);

	//! Registers a cast between two types
	GOOSE_API void RegisterCastFunction(const LogicalType &source, const LogicalType &target,
	                                     bind_cast_function_t function, int64_t implicit_cast_cost = -1);
	GOOSE_API void RegisterCastFunction(const LogicalType &source, const LogicalType &target, BoundCastInfo function,
	                                     int64_t implicit_cast_cost = -1);

private:
	void FinalizeLoad();

private:
	DatabaseInstance &db;
	string extension_name;
	string extension_description;
	optional_ptr<ExtensionInfo> extension_info;
};

} // namespace goose

//! Helper macro to define the entrypoint for a C++ extension
//! Usage:
//!
//!		GOOSE_CPP_EXTENSION_ENTRY(my_extension, loader) {
//!			loader.RegisterFunction(...);
//!		}
//!
#define GOOSE_CPP_EXTENSION_ENTRY(EXTENSION_NAME, LOADER_NAME)                                                        \
	GOOSE_EXTENSION_API void EXTENSION_NAME##_goose_cpp_init(goose::ExtensionLoader &LOADER_NAME)
