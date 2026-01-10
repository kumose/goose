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

#include <goose/planner/column_binding_map.h>
#include <goose/planner/logical_operator_visitor.h>

namespace goose {
//! The CommonAggregateOptimizer optimizer eliminates duplicate aggregates from aggregate nodes
class CommonAggregateOptimizer : public LogicalOperatorVisitor {
public:
	void VisitOperator(LogicalOperator &op) override;

private:
	void StandardVisitOperator(LogicalOperator &op);
	unique_ptr<Expression> VisitReplace(BoundColumnRefExpression &expr, unique_ptr<Expression> *expr_ptr) override;
	void ExtractCommonAggregates(LogicalAggregate &aggr);

private:
	column_binding_map_t<ColumnBinding> aggregate_map;
};
} // namespace goose
