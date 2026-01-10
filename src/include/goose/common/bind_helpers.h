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

#include <goose/common/vector.h>
#include <goose/common/common.h>

namespace goose {

class Value;

Value ConvertVectorToValue(vector<Value> set);
vector<bool> ParseColumnList(const vector<Value> &set, vector<string> &names, const string &option_name);
vector<bool> ParseColumnList(const Value &value, vector<string> &names, const string &option_name);
vector<idx_t> ParseColumnsOrdered(const vector<Value> &set, vector<string> &names, const string &loption);
vector<idx_t> ParseColumnsOrdered(const Value &value, vector<string> &names, const string &loption);

} // namespace goose
