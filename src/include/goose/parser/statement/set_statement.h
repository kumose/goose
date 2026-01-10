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
