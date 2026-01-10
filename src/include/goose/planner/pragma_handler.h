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

#include <goose/common/common.h>
#include <goose/common/vector.h>
#include <goose/parser/statement/pragma_statement.h>

namespace goose {
class ClientContext;
class ClientContextLock;
class SQLStatement;
struct PragmaInfo;

//! Pragma handler is responsible for converting certain pragma statements into new queries
class PragmaHandler {
public:
	explicit PragmaHandler(ClientContext &context);

	void HandlePragmaStatements(ClientContextLock &lock, vector<unique_ptr<SQLStatement>> &statements);

private:
	ClientContext &context;

private:
	//! Handles a pragma statement, returns whether the statement was expanded, if it was expanded the 'resulting_query'
	//! contains the statement(s) to replace the current one
	bool HandlePragma(SQLStatement &statement, string &resulting_query);

	void HandlePragmaStatementsInternal(vector<unique_ptr<SQLStatement>> &statements);
};
} // namespace goose
