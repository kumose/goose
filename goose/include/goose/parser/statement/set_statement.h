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

#include <goose/common/enums/set_scope.h>
#include <goose/common/enums/set_type.h>
#include <goose/parser/sql_statement.h>
#include <goose/parser/parsed_expression.h>

namespace goose {

class SetStatement : public SQLStatement {
public:
	static constexpr const StatementType TYPE = StatementType::SET_STATEMENT;

protected:
	SetStatement(string name_p, SetScope scope_p, SetType type_p);
	SetStatement(const SetStatement &other) = default;

public:
	string name;
	SetScope scope;
	SetType set_type;
};

class SetVariableStatement : public SetStatement {
public:
	SetVariableStatement(string name_p, unique_ptr<ParsedExpression> value_p, SetScope scope_p);

protected:
	SetVariableStatement(const SetVariableStatement &other);

public:
	unique_ptr<SQLStatement> Copy() const override;
	string ToString() const override;

public:
	unique_ptr<ParsedExpression> value;
};

class ResetVariableStatement : public SetStatement {
public:
	ResetVariableStatement(std::string name_p, SetScope scope_p);

protected:
	ResetVariableStatement(const ResetVariableStatement &other) = default;

public:
	unique_ptr<SQLStatement> Copy() const override;
	string ToString() const override;
};

} // namespace goose
