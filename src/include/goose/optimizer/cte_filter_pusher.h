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
