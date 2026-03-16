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
#include <goose/parser/parsed_expression.h>
#include <goose/parser/query_node.h>
#include <goose/parser/column_list.h>
#include <goose/parser/simplified_token.h>
#include <goose/parser/parser_options.h>
#include <goose/common/exception/parser_exception.h>
#include <goose/parser/parser_extension.h>

namespace goose_libpgquery {
struct PGNode;
struct PGList;
} // namespace goose_libpgquery

namespace goose {

class GroupByNode;

//! The parser is responsible for parsing the query and converting it into a set
//! of parsed statements. The parsed statements can then be converted into a
//! plan and executed.
class Parser {
public:
	explicit Parser(ParserOptions options = ParserOptions());

	//! The parsed SQL statements from an invocation to ParseQuery.
	vector<unique_ptr<SQLStatement>> statements;

public:
	//! Attempts to parse a query into a series of SQL statements. Returns
	//! whether or not the parsing was successful. If the parsing was
	//! successful, the parsed statements will be stored in the statements
	//! variable.
	void ParseQuery(const string &query);

	//! Tokenize a query, returning the raw tokens together with their locations
	static vector<SimplifiedToken> Tokenize(const string &query);

	//! Tokenize an error message, returning the raw tokens together with their locations
	static vector<SimplifiedToken> TokenizeError(const string &error_msg);

	//! Returns true if the given text matches a keyword of the parser
	static KeywordCategory IsKeyword(const string &text);
	//! Returns a list of all keywords in the parser
	static vector<ParserKeyword> KeywordList();

	//! Parses a list of expressions (i.e. the list found in a SELECT clause)
	GOOSE_API static vector<unique_ptr<ParsedExpression>> ParseExpressionList(const string &select_list,
	                                                                           ParserOptions options = ParserOptions());
	//! Parses a list of GROUP BY expressions
	static GroupByNode ParseGroupByList(const string &group_by, ParserOptions options = ParserOptions());
	//! Parses a list as found in an ORDER BY expression (i.e. including optional ASCENDING/DESCENDING modifiers)
	static vector<OrderByNode> ParseOrderList(const string &select_list, ParserOptions options = ParserOptions());
	//! Parses an update list (i.e. the list found in the SET clause of an UPDATE statement)
	static void ParseUpdateList(const string &update_list, vector<string> &update_columns,
	                            vector<unique_ptr<ParsedExpression>> &expressions,
	                            ParserOptions options = ParserOptions());
	//! Parses a VALUES list (i.e. the list of expressions after a VALUES clause)
	static vector<vector<unique_ptr<ParsedExpression>>> ParseValuesList(const string &value_list,
	                                                                    ParserOptions options = ParserOptions());
	//! Parses a column list (i.e. as found in a CREATE TABLE statement)
	static ColumnList ParseColumnList(const string &column_list, ParserOptions options = ParserOptions());
	static ColumnDefinition ParseColumnDefinition(const string &column_definition,
	                                              ParserOptions options = ParserOptions());

	static bool StripUnicodeSpaces(const string &query_str, string &new_query);

	unique_ptr<SQLStatement> GetStatement(const string &query);
	void ThrowParserOverrideError(ParserOverrideResult &result);

private:
	ParserOptions options;
};
} // namespace goose
