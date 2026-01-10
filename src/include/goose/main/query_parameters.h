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

enum class QueryResultOutputType : uint8_t { FORCE_MATERIALIZED, ALLOW_STREAMING };

enum class QueryResultMemoryType : uint8_t { IN_MEMORY, BUFFER_MANAGED };

struct QueryParameters {
	QueryParameters() {
	}
	QueryParameters(bool allow_streaming) // NOLINT: allow implicit conversion
	    : output_type(allow_streaming ? QueryResultOutputType::ALLOW_STREAMING
	                                  : QueryResultOutputType::FORCE_MATERIALIZED) {
	}
	QueryParameters(QueryResultOutputType output_type) // NOLINT: allow implicit conversion
	    : output_type(output_type) {
	}
	QueryResultOutputType output_type = QueryResultOutputType::FORCE_MATERIALIZED;
	QueryResultMemoryType memory_type = QueryResultMemoryType::IN_MEMORY;
};

} // namespace goose
