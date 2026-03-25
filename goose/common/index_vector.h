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

#include <goose/common/constants.h>
#include <goose/common/vector.h>
#include <goose/common/serializer/serializer.h>
#include <goose/common/serializer/deserializer.h>

namespace goose {
    template<class T, class INDEX_TYPE>
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

    template<typename T>
    using physical_index_vector_t = IndexVector<T, PhysicalIndex>;

    template<typename T>
    using logical_index_vector_t = IndexVector<T, LogicalIndex>;
} // namespace goose
