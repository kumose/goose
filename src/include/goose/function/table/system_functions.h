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
