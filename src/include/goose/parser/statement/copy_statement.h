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

#include <goose/parser/parsed_data/copy_info.h>
#include <goose/parser/query_node.h>
#include <goose/parser/sql_statement.h>

namespace goose {

enum class CopyToType : uint8_t { COPY_TO_FILE, EXPORT_DATABASE };

class CopyStatement : public SQLStatement {
public:
	static constexpr const StatementType TYPE = StatementType::COPY_STATEMENT;

public:
	CopyStatement();

	unique_ptr<CopyInfo> info;

	string ToString() const override;

protected:
	CopyStatement(const CopyStatement &other);

public:
	GOOSE_API unique_ptr<SQLStatement> Copy() const override;

private:
};
} // namespace goose
