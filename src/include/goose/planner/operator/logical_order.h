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

#include <goose/planner/bound_query_node.h>
#include <goose/planner/expression/bound_reference_expression.h>
#include <goose/planner/logical_operator.h>
#include <goose/storage/statistics/base_statistics.h>

namespace goose {

//! LogicalOrder represents an ORDER BY clause, sorting the data
class LogicalOrder : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_ORDER_BY;

public:
	explicit LogicalOrder(vector<BoundOrderByNode> orders);

	vector<BoundOrderByNode> orders;
	vector<idx_t> projection_map;

public:
	vector<ColumnBinding> GetColumnBindings() override;

	bool HasProjectionMap() const override {
		return !projection_map.empty();
	}

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	InsertionOrderPreservingMap<string> ParamsToString() const override;

protected:
	void ResolveTypes() override;
};
} // namespace goose
