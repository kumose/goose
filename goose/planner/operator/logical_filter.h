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

//! LogicalFilter represents a filter operation (e.g. WHERE or HAVING clause)
class LogicalFilter : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_FILTER;

public:
	explicit LogicalFilter(unique_ptr<Expression> expression);
	LogicalFilter();

	vector<idx_t> projection_map;

public:
	vector<ColumnBinding> GetColumnBindings() override;

	bool HasProjectionMap() const override {
		return !projection_map.empty();
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	bool SplitPredicates() {
		return SplitPredicates(expressions);
	}
	//! Splits up the predicates of the LogicalFilter into a set of predicates
	//! separated by AND Returns whether or not any splits were made
	static bool SplitPredicates(vector<unique_ptr<Expression>> &expressions);

protected:
	void ResolveTypes() override;
};

} // namespace goose
