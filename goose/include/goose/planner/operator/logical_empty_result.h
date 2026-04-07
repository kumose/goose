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

//! LogicalEmptyResult returns an empty result. This is created by the optimizer if it can reason that certain parts of
//! the tree will always return an empty result.
class LogicalEmptyResult : public LogicalOperator {
	LogicalEmptyResult();

public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_EMPTY_RESULT;

public:
	explicit LogicalEmptyResult(unique_ptr<LogicalOperator> op);
	LogicalEmptyResult(vector<LogicalType> return_types, vector<ColumnBinding> bindings);

	//! The set of return types of the empty result
	vector<LogicalType> return_types;
	//! The columns that would be bound at this location (if the subtree was not optimized away)
	vector<ColumnBinding> bindings;

public:
	vector<ColumnBinding> GetColumnBindings() override {
		return bindings;
	}
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
	idx_t EstimateCardinality(ClientContext &context) override {
		return 0;
	}

protected:
	void ResolveTypes() override {
		this->types = return_types;
	}
};
} // namespace goose
