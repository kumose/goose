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
#include <goose/common/arrow/arrow.h>
#include <goose/common/helper.h>

//! Here we have the internal goose classes that interact with Arrow's Internal Header (i.e., goose/commons/arrow.hpp)
namespace goose {

class ArrowSchemaWrapper {
public:
	ArrowSchema arrow_schema;

	ArrowSchemaWrapper() {
		arrow_schema.release = nullptr;
	}

	~ArrowSchemaWrapper();
};
class ArrowArrayWrapper {
public:
	ArrowArray arrow_array;
	ArrowArrayWrapper() {
		arrow_array.length = 0;
		arrow_array.release = nullptr;
	}
	ArrowArrayWrapper(ArrowArrayWrapper &&other) noexcept : arrow_array(other.arrow_array) {
		other.arrow_array.release = nullptr;
	}
	ArrowArrayWrapper &operator=(ArrowArrayWrapper &&other) noexcept {
		if (this != &other) {
			if (arrow_array.release) {
				arrow_array.release(&arrow_array);
			}
			arrow_array = other.arrow_array;
			other.arrow_array.release = nullptr;
		}
		return *this;
	}
	~ArrowArrayWrapper();
};

class ArrowArrayStreamWrapper {
public:
	ArrowArrayStream arrow_array_stream;
	int64_t number_of_rows;

public:
	void GetSchema(ArrowSchemaWrapper &schema);

	virtual shared_ptr<ArrowArrayWrapper> GetNextChunk();

	const char *GetError();

	virtual ~ArrowArrayStreamWrapper();
	ArrowArrayStreamWrapper() {
		arrow_array_stream.release = nullptr;
	}
};

} // namespace goose
