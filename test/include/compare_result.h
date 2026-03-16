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

#include <goose/common/string_util.h>
#include <goose/goose.h>

namespace goose {

bool CHECK_COLUMN(QueryResult &result, size_t column_number, vector<goose::Value> values);
bool CHECK_COLUMN(goose::unique_ptr<goose::QueryResult> &result, size_t column_number, vector<goose::Value> values);
bool CHECK_COLUMN(goose::unique_ptr<goose::MaterializedQueryResult> &result, size_t column_number,
                  vector<goose::Value> values);

string compare_csv(goose::QueryResult &result, string csv, bool header = false);
string compare_csv_collection(goose::ColumnDataCollection &collection, string csv, bool header = false);

bool parse_datachunk(string csv, DataChunk &result, vector<LogicalType> sql_types, bool has_header);

//! Compares the result of a pipe-delimited CSV with the given DataChunk
//! Returns true if they are equal, and stores an error_message otherwise
bool compare_result(string csv, ColumnDataCollection &collection, vector<LogicalType> sql_types, bool has_header,
                    string &error_message);

} // namespace goose
