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
