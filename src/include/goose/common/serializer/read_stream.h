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
#include <goose/main/client_context.h>

#include <type_traits>

namespace goose {

class ReadStream {
public:
	// Reads a set amount of data from the stream into the specified buffer and moves the stream forward accordingly
	virtual void ReadData(data_ptr_t buffer, idx_t read_size) = 0;
	virtual void ReadData(QueryContext context, data_ptr_t buffer, idx_t read_size) = 0;

	// Reads a type from the stream and moves the stream forward sizeof(T) bytes
	// The type must be a standard layout type
	template <class T>
	T Read() {
		return Read<T>(QueryContext());
	}

	template <class T>
	T Read(QueryContext context) {
		static_assert(std::is_standard_layout<T>(), "Read element must be a standard layout data type");
		T value;
		ReadData(context, data_ptr_cast(&value), sizeof(T));
		return value;
	}

	virtual ~ReadStream() {
	}
};

} // namespace goose
