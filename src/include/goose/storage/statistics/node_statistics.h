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

#include <goose/common/common.h>

namespace goose {

class NodeStatistics {
public:
	NodeStatistics() : has_estimated_cardinality(false), has_max_cardinality(false) {
	}
	explicit NodeStatistics(idx_t estimated_cardinality)
	    : has_estimated_cardinality(true), estimated_cardinality(estimated_cardinality), has_max_cardinality(false) {
	}
	NodeStatistics(idx_t estimated_cardinality, idx_t max_cardinality)
	    : has_estimated_cardinality(true), estimated_cardinality(estimated_cardinality), has_max_cardinality(true),
	      max_cardinality(max_cardinality) {
	}

	//! Whether or not the node has an estimated cardinality specified
	bool has_estimated_cardinality;
	//! The estimated cardinality at the specified node
	idx_t estimated_cardinality;
	//! Whether or not the node has a maximum cardinality specified
	bool has_max_cardinality;
	//! The max possible cardinality at the specified node
	idx_t max_cardinality;
};

} // namespace goose
