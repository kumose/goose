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
