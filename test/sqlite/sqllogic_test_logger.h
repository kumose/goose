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
#include <goose/common/types-import.h>
#include "sqllogic_command.h"

namespace goose {

class Command;
class LoopCommand;

class SQLLogicTestLogger {
public:
	SQLLogicTestLogger(ExecuteContext &context, const Command &command);
	~SQLLogicTestLogger();

	static void Log(const string &annotation, const string &str);
	void PrintExpectedResult(const vector<string> &values, idx_t columns, bool row_wise);
	static void PrintLineSep();
	static void PrintHeader(string header);
	void PrintFileHeader();
	void PrintSQL();
	void PrintSQLFormatted();
	void PrintErrorHeader(const string &description);
	static void PrintErrorHeader(const string &file_name, idx_t query_line, const string &description);
	void PrintResultError(const vector<string> &result_values, const vector<string> &values,
	                      idx_t expected_column_count, bool row_wise);
	static void PrintSummaryHeader(const std::string &file_name, idx_t query_line);
	void PrintResultError(MaterializedQueryResult &result, const vector<string> &values, idx_t expected_column_count,
	                      bool row_wise);
	void PrintResultString(MaterializedQueryResult &result);
	void UnexpectedFailure(MaterializedQueryResult &result);
	void OutputResult(MaterializedQueryResult &result, const vector<string> &result_values_string);
	void OutputHash(const string &hash_value);
	void ColumnCountMismatch(MaterializedQueryResult &result, const vector<string> &result_values_string,
	                         idx_t expected_column_count, bool row_wise);
	void NotCleanlyDivisible(idx_t expected_column_count, idx_t actual_column_count);
	void WrongRowCount(idx_t expected_rows, MaterializedQueryResult &result, const vector<string> &comparison_values,
	                   idx_t expected_column_count, bool row_wise);
	void ColumnCountMismatchCorrectResult(idx_t original_expected_columns, idx_t expected_column_count,
	                                      MaterializedQueryResult &result);
	void SplitMismatch(idx_t row_number, idx_t expected_column_count, idx_t split_count);
	void WrongResultHash(QueryResult *expected_result, MaterializedQueryResult &result, const string &expected_hash,
	                     const string &actual_hash);
	void UnexpectedStatement(bool expect_ok, MaterializedQueryResult &result);
	void ExpectedErrorMismatch(const string &expected_error, MaterializedQueryResult &result);
	void InternalException(MaterializedQueryResult &result);
	static void LoadDatabaseFail(const string &file_name, const string &dbpath, const string &message);

	static void AppendFailure(const string &log_message);
	static void LogFailure(const string &log_message);
	static void LogFailureAnnotation(const string &log_message);

private:
	lock_guard<mutex> log_lock;
	string file_name;
	int query_line;
	string sql_query;
};
} // namespace goose
