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

#include <goose/common/insertion_order_preserving_map.h>

namespace goose {

class LogicalOperator;
class Optimizer;

class CTEFilterPusher {
public:
	explicit CTEFilterPusher(Optimizer &optimizer);
	//! Finds all materialized CTEs and pushes OR filters into them (if applicable)
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);

private:
	//! CTE info needed for creating OR filters that can be pushed down
	struct MaterializedCTEInfo {
		explicit MaterializedCTEInfo(LogicalOperator &materialized_cte);
		LogicalOperator &materialized_cte;
		vector<reference<LogicalOperator>> filters;
		bool all_cte_refs_are_filtered;
	};

private:
	//! Find all materialized CTEs and their refs
	void FindCandidates(LogicalOperator &op);
	//! Creates an OR filter and pushes it into a materialized CTE
	void PushFilterIntoCTE(MaterializedCTEInfo &info);

private:
	//! The optimizer
	Optimizer &optimizer;
	//! Mapping from CTE index to CTE info, order preserving so deepest CTEs are done first
	InsertionOrderPreservingMap<unique_ptr<MaterializedCTEInfo>> cte_info_map;
};

} // namespace goose
