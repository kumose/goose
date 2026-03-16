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
