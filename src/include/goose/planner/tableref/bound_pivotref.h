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

#include <goose/planner/binder.h>
#include <goose/planner/expression.h>
#include <goose/parser/tableref/pivotref.h>
#include <goose/function/aggregate_function.h>

namespace goose {

struct BoundPivotInfo {
	//! The number of group columns
	idx_t group_count;
	//! The set of types
	vector<LogicalType> types;
	//! The set of values to pivot on
	vector<string> pivot_values;
	//! The set of aggregate functions that is being executed
	vector<unique_ptr<Expression>> aggregates;

	void Serialize(Serializer &serializer) const;
	static BoundPivotInfo Deserialize(Deserializer &deserializer);
};

class BoundPivotRef {
public:
	idx_t bind_index;
	//! The binder used to bind the child of the pivot
	shared_ptr<Binder> child_binder;
	//! The child node of the pivot
	BoundStatement child;
	//! The bound pivot info
	BoundPivotInfo bound_pivot;
};
} // namespace goose
