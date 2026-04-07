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

#include <goose/common/common.h>
#include <goose/common/types-import.h>

namespace goose {
    class Expression;

    template<class T>
    struct ReferenceHashFunction {
        uint64_t operator()(const reference<T> &ref) const {
            return std::hash<void *>()((void *) &ref.get());
        }
    };

    template<class T>
    struct ReferenceEquality {
        bool operator()(const reference<T> &a, const reference<T> &b) const {
            return &a.get() == &b.get();
        }
    };

    template<typename T, typename TGT>
    using reference_map_t = unordered_map<reference<T>, TGT, ReferenceHashFunction<T>, ReferenceEquality<T> >;

    template<typename T>
    using reference_set_t = unordered_set<reference<T>, ReferenceHashFunction<T>, ReferenceEquality<T> >;
} // namespace goose
