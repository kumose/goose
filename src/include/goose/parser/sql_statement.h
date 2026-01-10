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

#include <goose/common/enums/statement_type.h>
#include <goose/common/exception.h>
#include <goose/common/printer.h>
#include <goose/common/named_parameter_map.h>

namespace goose {

//! SQLStatement is the base class of any type of SQL statement.
class SQLStatement {
public:
	static constexpr const StatementType TYPE = StatementType::INVALID_STATEMENT;

public:
	explicit SQLStatement(StatementType type) : type(type) {
	}
	virtual ~SQLStatement() {
	}

	//! The statement type
	StatementType type;
	//! The statement location within the query string
	idx_t stmt_location = 0;
	//! The statement length within the query string
	idx_t stmt_length = 0;
	//! The map of named parameter to param index
	case_insensitive_map_t<idx_t> named_param_map;
	//! The query text that corresponds to this SQL statement
	string query;

protected:
	SQLStatement(const SQLStatement &other) = default;

public:
	virtual string ToString() const = 0;
	//! Create a copy of this SelectStatement
	GOOSE_API virtual unique_ptr<SQLStatement> Copy() const = 0;

public:
public:
	template <class TARGET>
	TARGET &Cast() {
		if (type != TARGET::TYPE && TARGET::TYPE != StatementType::INVALID_STATEMENT) {
			throw InternalException("Failed to cast statement to type - statement type mismatch");
		}
		return reinterpret_cast<TARGET &>(*this);
	}

	template <class TARGET>
	const TARGET &Cast() const {
		if (type != TARGET::TYPE && TARGET::TYPE != StatementType::INVALID_STATEMENT) {
			throw InternalException("Failed to cast statement to type - statement type mismatch");
		}
		return reinterpret_cast<const TARGET &>(*this);
	}
};
} // namespace goose
