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
#include <goose/common/types-import.h>
#include <goose/testing/sqlite/sqllogic_command.h>

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

        void PrintResultError(MaterializedQueryResult &result, const vector<string> &values,
                              idx_t expected_column_count,
                              bool row_wise);

        void PrintResultString(MaterializedQueryResult &result);

        void UnexpectedFailure(MaterializedQueryResult &result);

        void OutputResult(MaterializedQueryResult &result, const vector<string> &result_values_string);

        void OutputHash(const string &hash_value);

        void ColumnCountMismatch(MaterializedQueryResult &result, const vector<string> &result_values_string,
                                 idx_t expected_column_count, bool row_wise);

        void NotCleanlyDivisible(idx_t expected_column_count, idx_t actual_column_count);

        void WrongRowCount(idx_t expected_rows, MaterializedQueryResult &result,
                           const vector<string> &comparison_values,
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
