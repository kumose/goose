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
