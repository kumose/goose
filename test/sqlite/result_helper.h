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
