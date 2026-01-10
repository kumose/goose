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

#include <goose/common/constants.h>
#include <goose/common/vector.h>
#include <goose/common/serializer/serializer.h>
#include <goose/common/serializer/deserializer.h>

namespace goose {

template <class T, class INDEX_TYPE>
class IndexVector {
public:
	void push_back(T element) { // NOLINT: match stl API
		internal_vector.push_back(std::move(element));
	}

	T &operator[](INDEX_TYPE idx) {
		return internal_vector[idx.index];
	}

	const T &operator[](INDEX_TYPE idx) const {
		return internal_vector[idx.index];
	}

	idx_t size() const { // NOLINT: match stl API
		return internal_vector.size();
	}

	bool empty() const { // NOLINT: match stl API
		return internal_vector.empty();
	}

	void reserve(idx_t size) { // NOLINT: match stl API
		internal_vector.reserve(size);
	}

	typename vector<T>::iterator begin() { // NOLINT: match stl API
		return internal_vector.begin();
	}
	typename vector<T>::iterator end() { // NOLINT: match stl API
		return internal_vector.end();
	}
	typename vector<T>::const_iterator cbegin() { // NOLINT: match stl API
		return internal_vector.cbegin();
	}
	typename vector<T>::const_iterator cend() { // NOLINT: match stl API
		return internal_vector.cend();
	}
	typename vector<T>::const_iterator begin() const { // NOLINT: match stl API
		return internal_vector.begin();
	}
	typename vector<T>::const_iterator end() const { // NOLINT: match stl API
		return internal_vector.end();
	}

	void Serialize(Serializer &serializer) const {
		serializer.WriteProperty(100, "internal_vector", internal_vector);
	}

	static IndexVector<T, INDEX_TYPE> Deserialize(Deserializer &deserializer) {
		IndexVector<T, INDEX_TYPE> result;
		deserializer.ReadProperty(100, "internal_vector", result.internal_vector);
		return result;
	}

private:
	vector<T> internal_vector;
};

template <typename T>
using physical_index_vector_t = IndexVector<T, PhysicalIndex>;

template <typename T>
using logical_index_vector_t = IndexVector<T, LogicalIndex>;

} // namespace goose
