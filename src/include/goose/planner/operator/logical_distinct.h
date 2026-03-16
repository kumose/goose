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
#include <goose/planner/bound_result_modifier.h>

namespace goose {

//! LogicalDistinct filters duplicate entries from its child operator
class LogicalDistinct : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_DISTINCT;

public:
	explicit LogicalDistinct(DistinctType distinct_type);
	explicit LogicalDistinct(vector<unique_ptr<Expression>> targets, DistinctType distinct_type);

	//! Whether or not this is a DISTINCT or DISTINCT ON
	DistinctType distinct_type;
	//! The set of distinct targets
	vector<unique_ptr<Expression>> distinct_targets;
	//! The order by modifier (optional, only for distinct on)
	unique_ptr<BoundOrderModifier> order_by;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;

	vector<ColumnBinding> GetColumnBindings() override {
		return children[0]->GetColumnBindings();
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

protected:
	void ResolveTypes() override;
};
} // namespace goose
