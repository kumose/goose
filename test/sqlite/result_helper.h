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

#include <goose/goose.h>
#include "sqllogic_command.h"

namespace goose {
class SQLLogicTestRunner;
class SQLLogicTestLogger;

class TestResultHelper {
public:
	TestResultHelper(SQLLogicTestRunner &runner) : runner(runner) {
	}

	SQLLogicTestRunner &runner;

public:
	bool CheckQueryResult(const Query &query, ExecuteContext &context,
	                      goose::unique_ptr<MaterializedQueryResult> owned_result);
	bool CheckStatementResult(const Statement &statement, ExecuteContext &context,
	                          goose::unique_ptr<MaterializedQueryResult> owned_result);
	string SQLLogicTestConvertValue(Value value, LogicalType sql_type, bool original_sqlite_test);
	void GooseConvertResult(MaterializedQueryResult &result, bool original_sqlite_test, vector<string> &out_result);

	static bool ResultIsHash(const string &result);
	static bool ResultIsFile(string result);
	void SortQueryResult(SortStyle sort_style, vector<string> &result, idx_t ncols);

	bool MatchesRegex(SQLLogicTestLogger &logger, string lvalue_str, string rvalue_str);
	bool CompareValues(SQLLogicTestLogger &logger, MaterializedQueryResult &result, string lvalue_str,
	                   string rvalue_str, idx_t current_row, idx_t current_column, vector<string> &values,
	                   idx_t expected_column_count, bool row_wise, vector<string> &result_values,
	                   bool print_error = true);
	bool SkipErrorMessage(const string &message);
	bool SkipLoggingSameError(const string &file_name);

	vector<string> LoadResultFromFile(string fname, vector<string> names, idx_t &expected_column_count, string &error);
};

} // namespace goose
