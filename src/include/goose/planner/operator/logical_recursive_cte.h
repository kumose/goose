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

#include <goose/planner/operator/logical_cte.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/binder.h>

namespace goose {

class LogicalRecursiveCTE : public LogicalCTE {
	LogicalRecursiveCTE() : LogicalCTE(LogicalOperatorType::LOGICAL_RECURSIVE_CTE) {
	}

public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_RECURSIVE_CTE;

public:
	LogicalRecursiveCTE(string ctename_p, idx_t table_index, idx_t column_count, bool union_all,
	                    vector<unique_ptr<Expression>> key_targets, unique_ptr<LogicalOperator> top,
	                    unique_ptr<LogicalOperator> bottom)
	    : LogicalCTE(std::move(ctename_p), table_index, column_count, std::move(top), std::move(bottom),
	                 LogicalOperatorType::LOGICAL_RECURSIVE_CTE),
	      union_all(union_all), key_targets(std::move(key_targets)) {
	}

	bool union_all;
	// Flag if recurring table is referenced, if not we do not copy ht into ColumnDataCollection
	bool ref_recurring;
	vector<unique_ptr<Expression>> key_targets;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;

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
