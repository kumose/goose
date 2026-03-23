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
