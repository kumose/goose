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

#include <goose/common/exception.h>

namespace goose {
    class optional_idx {
        static constexpr const idx_t INVALID_INDEX = idx_t(-1);

    public:
        optional_idx() : index(INVALID_INDEX) {
        }

        optional_idx(idx_t index) : index(index) { // NOLINT: allow implicit conversion from idx_t
            if (index == INVALID_INDEX) {
                throw InternalException("optional_idx cannot be initialized with an invalid index");
            }
        }

        static optional_idx Invalid() {
            return optional_idx();
        }

        bool IsValid() const {
            return index != INVALID_INDEX;
        }

        void SetInvalid() {
            index = INVALID_INDEX;
        }

        idx_t GetIndex() const {
            if (index == INVALID_INDEX) {
                throw InternalException("Attempting to get the index of an optional_idx that is not set");
            }
            return index;
        }

        inline bool operator==(const optional_idx &rhs) const {
            return index == rhs.index;
        }

        inline bool operator!=(const optional_idx &rhs) const {
            return index != rhs.index;
        }

    private:
        idx_t index;
    };
} // namespace goose
