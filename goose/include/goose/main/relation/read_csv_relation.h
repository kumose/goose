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

#include <goose/execution/operator/csv_scanner/csv_reader_options.h>
#include <goose/main/relation/table_function_relation.h>
#include <goose/common/shared_ptr.h>
#include <goose/common/case_insensitive_map.h>

namespace goose {

class ReadCSVRelation : public TableFunctionRelation {
public:
	ReadCSVRelation(const shared_ptr<ClientContext> &context, const vector<string> &csv_files,
	                named_parameter_map_t &&options, string alias = string());

	string alias;

protected:
	void InitializeAlias(const vector<string> &input);

public:
	string GetAlias() override;
};

} // namespace goose
