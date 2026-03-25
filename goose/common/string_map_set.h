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

#include <goose/common/operator/comparison_operators.h>
#include <goose/common/types/hash.h>
#include <goose/common/types/string_type.h>
#include <goose/common/types-import.h>

namespace goose {
    struct StringHash {
        std::size_t operator()(const string_t &k) const {
            return Hash(k);
        }
    };

    struct StringEquality {
        bool operator()(const string_t &a, const string_t &b) const {
            return Equals::Operation(a, b);
        }
    };

    struct StringCIHash {
        std::size_t operator()(const string_t &k) const {
            return StringUtil::CIHash(k.GetData(), k.GetSize());
        }
    };

    struct StringCIEquality {
        bool operator()(const string_t &a, const string_t &b) const {
            return StringUtil::CIEquals(a.GetData(), a.GetSize(), b.GetData(), b.GetSize());
        }
    };

    template<typename T>
    using string_map_t = unordered_map<string_t, T, StringHash, StringEquality>;

    using string_set_t = unordered_set<string_t, StringHash, StringEquality>;

    template<typename T>
    using case_insensitive_string_map_t = unordered_map<string_t, T, StringCIHash, StringCIEquality>;

    using case_insensitive_string_set_t = unordered_set<string_t, StringCIHash, StringCIEquality>;
} // namespace goose
