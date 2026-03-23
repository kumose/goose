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

#include <goose/planner/logical_operator.h>

namespace goose {

class LogicalSetOperation : public LogicalOperator {
	LogicalSetOperation(idx_t table_index, idx_t column_count, LogicalOperatorType type, bool setop_all,
	                    bool allow_out_of_order);

public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_INVALID;

public:
	LogicalSetOperation(idx_t table_index, idx_t column_count, unique_ptr<LogicalOperator> top,
	                    unique_ptr<LogicalOperator> bottom, LogicalOperatorType type, bool setop_all,
	                    bool allow_out_of_order = true);
	LogicalSetOperation(idx_t table_index, idx_t column_count, vector<unique_ptr<LogicalOperator>> children,
	                    LogicalOperatorType type, bool setop_all, bool allow_out_of_order = true);

	idx_t table_index;
	idx_t column_count;
	bool setop_all;
	//! Whether or not UNION statements can be executed out of order
	bool allow_out_of_order;

public:
	vector<ColumnBinding> GetColumnBindings() override {
		return GenerateColumnBindings(table_index, column_count);
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	void ResolveTypes() override {
		types = children[0]->types;
	}
};
} // namespace goose
