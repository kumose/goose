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
