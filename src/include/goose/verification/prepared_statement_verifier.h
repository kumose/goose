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

#include <goose/verification/statement_verifier.h>

namespace goose {

class PreparedStatementVerifier : public StatementVerifier {
public:
	explicit PreparedStatementVerifier(unique_ptr<SQLStatement> statement_p,
	                                   optional_ptr<case_insensitive_map_t<BoundParameterData>> parameters);
	static unique_ptr<StatementVerifier> Create(const SQLStatement &statement_p,
	                                            optional_ptr<case_insensitive_map_t<BoundParameterData>> parameters);

	bool Run(ClientContext &context, const string &query,
	         const std::function<unique_ptr<QueryResult>(const string &, unique_ptr<SQLStatement>,
	                                                     optional_ptr<case_insensitive_map_t<BoundParameterData>>)>
	             &run) override;

private:
	case_insensitive_map_t<unique_ptr<ParsedExpression>> values;
	unique_ptr<SQLStatement> prepare_statement;
	unique_ptr<SQLStatement> execute_statement;
	unique_ptr<SQLStatement> dealloc_statement;

private:
	void Extract();
	void ConvertConstants(unique_ptr<ParsedExpression> &child);
};

} // namespace goose
