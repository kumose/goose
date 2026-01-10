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

#include <goose/catalog/catalog.h>
#include <goose/common/common.h>
#include <goose/common/exception.h>
#include <goose/common/vector.h>
#include <type_traits>

namespace goose {

class WriteStream {
public:
	// Writes a set amount of data from the specified buffer into the stream and moves the stream forward accordingly
	virtual void WriteData(const_data_ptr_t buffer, idx_t write_size) = 0;

	// Writes a type into the stream and moves the stream forward sizeof(T) bytes
	// The type must be a standard layout type
	template <class T>
	void Write(T element) {
		static_assert(std::is_standard_layout<T>(), "Write element must be a standard layout data type");
		WriteData(const_data_ptr_cast(&element), sizeof(T));
	}

	virtual ~WriteStream() {
	}
};

} // namespace goose
