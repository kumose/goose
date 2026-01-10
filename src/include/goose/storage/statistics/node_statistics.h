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
