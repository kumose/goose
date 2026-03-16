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
