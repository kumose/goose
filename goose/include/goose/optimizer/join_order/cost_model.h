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
