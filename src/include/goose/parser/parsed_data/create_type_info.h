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

#include <goose/parser/parsed_data/create_info.h>
#include <goose/parser/statement/select_statement.h>

namespace goose {

struct BindLogicalTypeInput {
	ClientContext &context;
	const LogicalType &base_type;
	const vector<Value> &modifiers;
};

//! The type to bind type modifiers to a type
typedef LogicalType (*bind_logical_type_function_t)(const BindLogicalTypeInput &input);

struct CreateTypeInfo : public CreateInfo {
	CreateTypeInfo();
	CreateTypeInfo(string name_p, LogicalType type_p, bind_logical_type_function_t bind_function_p = nullptr);

	//! Name of the Type
	string name;
	//! Logical Type
	LogicalType type;
	//! Used by create enum from query
	unique_ptr<SQLStatement> query;
	//! Bind type modifiers to the type
	bind_logical_type_function_t bind_function;

public:
	unique_ptr<CreateInfo> Copy() const override;

	GOOSE_API void Serialize(Serializer &serializer) const override;
	GOOSE_API static unique_ptr<CreateInfo> Deserialize(Deserializer &deserializer);

	string ToString() const override;
};

} // namespace goose
