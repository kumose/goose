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

#include <goose/common/enums/set_operation_type.h>
#include <goose/planner/binder.h>
#include <goose/planner/bound_query_node.h>

namespace goose {

//! Bound equivalent of SetOperationNode
class BoundSetOperationNode : public BoundQueryNode {
public:
	//! The type of set operation
	SetOperationType setop_type = SetOperationType::NONE;
	//! whether the ALL modifier was used or not
	bool setop_all = false;
	//! The bound children
	vector<BoundStatement> bound_children;
	//! Child binders
	vector<shared_ptr<Binder>> child_binders;

	//! Index used by the set operation
	idx_t setop_index;

public:
	idx_t GetRootIndex() override {
		return setop_index;
	}
};

} // namespace goose
