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

#include <goose/planner/table_filter.h>
#include <goose/common/common.h>
#include <goose/common/types-import.h>
#include <goose/common/random_engine.h>

namespace goose {

struct AdaptiveFilterState {
	time_point<high_resolution_clock> start_time;
};

class AdaptiveFilter {
public:
	explicit AdaptiveFilter(const Expression &expr);
	explicit AdaptiveFilter(const TableFilterSet &table_filters);

	vector<idx_t> permutation;

public:
	void AdaptRuntimeStatistics(double duration);

	AdaptiveFilterState BeginFilter() const;
	void EndFilter(AdaptiveFilterState state);

private:
	bool disable_permutations = false;

	//! used for adaptive expression reordering
	idx_t iteration_count = 0;
	idx_t swap_idx = 0;
	idx_t right_random_border = 0;
	idx_t observe_interval = 0;
	idx_t execute_interval = 0;
	double runtime_sum = 0;
	double prev_mean = 0;
	bool observe = false;
	bool warmup = false;
	vector<idx_t> swap_likeliness;
	RandomEngine generator;
};
} // namespace goose
