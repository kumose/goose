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

#include <goose/common/types.h>
#include <goose/common/types/vector_buffer.h>
#include <goose/common/vector.h>

namespace goose {
class Allocator;
class Vector;

//! The VectorCache holds cached vector data.
//! It enables re-using the same memory for different vectors.
class VectorCache {
public:
	//! Instantiate an empty vector cache.
	GOOSE_API VectorCache();
	//! Instantiate a vector cache with the given type and capacity.
	GOOSE_API VectorCache(Allocator &allocator, const LogicalType &type, const idx_t capacity = STANDARD_VECTOR_SIZE);

public:
	buffer_ptr<VectorBuffer> buffer;

public:
	void ResetFromCache(Vector &result) const;
	const LogicalType &GetType() const;
};

} // namespace goose
