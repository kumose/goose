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

#include <goose/optimizer/join_order/join_node.h>
#include <goose/common/enums/join_type.h>
#include <goose/optimizer/join_order/cardinality_estimator.h>

namespace goose {

class QueryGraphManager;

class CostModel {
public:
	explicit CostModel(QueryGraphManager &query_graph_manager);

private:
	//! query graph storing relation manager information
	QueryGraphManager &query_graph_manager;

public:
	void InitCostModel();

	//! Compute cost of a join relation set
	double ComputeCost(DPJoinNode &left, DPJoinNode &right);

	//! Cardinality Estimator used to calculate cost
	CardinalityEstimator cardinality_estimator;

private:
};

} // namespace goose
