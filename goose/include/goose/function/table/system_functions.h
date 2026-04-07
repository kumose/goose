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

struct PragmaCollations {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct PragmaTableInfo {
	static void GetColumnInfo(TableCatalogEntry &table, const ColumnDefinition &column, DataChunk &output, idx_t index);

	static void RegisterFunction(BuiltinFunctions &set);
};

struct PragmaStorageInfo {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct PragmaMetadataInfo {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct PragmaVersion {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct PragmaPlatform {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct PragmaDatabaseSize {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseSchemasFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseConnectionCountFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseApproxDatabaseCountFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseColumnsFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseConstraintsFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseSecretsFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseWhichSecretFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseDatabasesFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseDependenciesFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseExtensionsFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GoosePreparedStatementsFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseFunctionsFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseKeywordsFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseLogFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseLogContextFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseIndexesFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseMemoryFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseExternalFileCacheFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseOptimizersFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseSecretTypesFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseSequencesFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseSettingsFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseTablesFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseTableSample {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseTemporaryFilesFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseTypesFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseVariablesFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct GooseViewsFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct EnableLoggingFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct EnableProfilingFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct TestType {
	TestType(LogicalType type_p, string name_p)
	    : type(std::move(type_p)), name(std::move(name_p)), min_value(Value::MinimumValue(type)),
	      max_value(Value::MaximumValue(type)) {
	}
	TestType(LogicalType type_p, string name_p, Value min, Value max)
	    : type(std::move(type_p)), name(std::move(name_p)), min_value(std::move(min)), max_value(std::move(max)) {
	}

	LogicalType type;
	string name;
	Value min_value;
	Value max_value;
};

struct TestAllTypesFun {
	static void RegisterFunction(BuiltinFunctions &set);
	static vector<TestType> GetTestTypes(bool large_enum = false, bool large_bignum = false);
};

struct TestVectorTypesFun {
	static void RegisterFunction(BuiltinFunctions &set);
};

struct PragmaUserAgent {
	static void RegisterFunction(BuiltinFunctions &set);
};

} // namespace goose
