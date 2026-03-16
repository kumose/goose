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

#include <goose/function/function_set.h>
#include <goose/parser/parsed_data/alter_info.h>

namespace goose {

//===--------------------------------------------------------------------===//
// Alter Table Function
//===--------------------------------------------------------------------===//
enum class AlterTableFunctionType : uint8_t { INVALID = 0, ADD_FUNCTION_OVERLOADS = 1 };

struct AlterTableFunctionInfo : public AlterInfo {
	AlterTableFunctionInfo(AlterTableFunctionType type, AlterEntryData data);
	~AlterTableFunctionInfo() override;

	AlterTableFunctionType alter_table_function_type;

public:
	CatalogType GetCatalogType() const override;
};

//===--------------------------------------------------------------------===//
// AddTableFunctionOverloadInfo
//===--------------------------------------------------------------------===//
struct AddTableFunctionOverloadInfo : public AlterTableFunctionInfo {
	AddTableFunctionOverloadInfo(AlterEntryData data, TableFunctionSet new_overloads);
	~AddTableFunctionOverloadInfo() override;

	TableFunctionSet new_overloads;

public:
	unique_ptr<AlterInfo> Copy() const override;
	string ToString() const override;
};

} // namespace goose
