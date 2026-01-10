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

#include <goose/main/client_context.h>
#include <goose/main/materialized_query_result.h>
#include <goose/parser/statement/select_statement.h>

namespace goose {

enum class VerificationType : uint8_t {
	ORIGINAL,
	COPIED,
	DESERIALIZED,
	PARSED,
	UNOPTIMIZED,
	NO_OPERATOR_CACHING,
	PREPARED,
	EXTERNAL,
	EXPLAIN,
	FETCH_ROW_AS_SCAN,

	INVALID
};

class StatementVerifier {
public:
	StatementVerifier(VerificationType type, string name, unique_ptr<SQLStatement> statement_p,
	                  optional_ptr<case_insensitive_map_t<BoundParameterData>> values);
	explicit StatementVerifier(unique_ptr<SQLStatement> statement_p,
	                           optional_ptr<case_insensitive_map_t<BoundParameterData>> values);
	static unique_ptr<StatementVerifier> Create(VerificationType type, const SQLStatement &statement_p,
	                                            optional_ptr<case_insensitive_map_t<BoundParameterData>> values);
	virtual ~StatementVerifier() noexcept;

public:
	//! Check whether expressions in this verifier and the other verifier match
	void CheckExpressions(const StatementVerifier &other) const;
	//! Check whether expressions within this verifier match
	void CheckExpressions() const;

	//! Run the select statement and store the result
	virtual bool
	Run(ClientContext &context, const string &query,
	    const std::function<unique_ptr<QueryResult>(const string &, unique_ptr<SQLStatement>,
	                                                optional_ptr<case_insensitive_map_t<BoundParameterData>>)> &run);
	//! Compare this verifier's results with another verifier
	string CompareResults(const StatementVerifier &other);

public:
	virtual bool RequireEquality() const {
		return true;
	}

	virtual bool DisableOptimizer() const {
		return false;
	}

	virtual bool DisableOperatorCaching() const {
		return false;
	}

	virtual bool ForceExternal() const {
		return false;
	}

	virtual bool ForceFetchRow() const {
		return false;
	}

public:
	const VerificationType type;
	const string name;
	unique_ptr<SQLStatement> statement;
	optional_ptr<SelectStatement> select_statement;
	optional_ptr<case_insensitive_map_t<BoundParameterData>> parameters;
	const vector<unique_ptr<ParsedExpression>> &select_list;
	unique_ptr<MaterializedQueryResult> materialized_result;

private:
	const vector<unique_ptr<ParsedExpression>> empty_select_list = {};

	const vector<unique_ptr<ParsedExpression>> &GetSelectList(QueryNode &node);
};

} // namespace goose
