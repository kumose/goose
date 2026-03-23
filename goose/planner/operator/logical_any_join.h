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

#include <goose/parser/parsed_expression.h>
#include <goose/planner/operator/logical_join.h>

namespace goose {

//! LogicalAnyJoin represents a join with an arbitrary expression as JoinCondition
class LogicalAnyJoin : public LogicalJoin {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_ANY_JOIN;

public:
	explicit LogicalAnyJoin(JoinType type);

	//! The JoinCondition on which this join is performed
	unique_ptr<Expression> condition;

public:
	InsertionOrderPreservingMap<string> ParamsToString() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);
};

} // namespace goose
