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

#include <goose/planner/bound_query_node.h>
#include <goose/planner/logical_operator.h>

namespace goose {
struct DynamicFilterData;

//! LogicalTopN represents a comibination of ORDER BY and LIMIT clause, using Min/Max Heap
class LogicalTopN : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_TOP_N;

public:
	LogicalTopN(vector<BoundOrderByNode> orders, idx_t limit, idx_t offset);
	~LogicalTopN() override;

	vector<BoundOrderByNode> orders;
	//! The maximum amount of elements to emit
	idx_t limit;
	//! The offset from the start to begin emitting elements
	idx_t offset;
	//! Dynamic table filter (if any)
	shared_ptr<DynamicFilterData> dynamic_filter;

public:
	vector<ColumnBinding> GetColumnBindings() override {
		return children[0]->GetColumnBindings();
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	idx_t EstimateCardinality(ClientContext &context) override;

protected:
	void ResolveTypes() override {
		types = children[0]->types;
	}
};
} // namespace goose
