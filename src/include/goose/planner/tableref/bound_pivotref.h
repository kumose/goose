// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

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
