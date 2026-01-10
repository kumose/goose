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

#include <goose/parser/sql_statement.h>
#include <goose/parser/parsed_data/transaction_info.h>

namespace goose {

class TransactionStatement : public SQLStatement {
public:
	static constexpr const StatementType TYPE = StatementType::TRANSACTION_STATEMENT;

public:
	explicit TransactionStatement(unique_ptr<TransactionInfo> info);

	unique_ptr<TransactionInfo> info;

protected:
	TransactionStatement(const TransactionStatement &other);

public:
	unique_ptr<SQLStatement> Copy() const override;
	string ToString() const override;
};
} // namespace goose
