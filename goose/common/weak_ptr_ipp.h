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
//


#pragma once

#include <goose/common/shared_ptr_ipp.h>

namespace goose {
    template<typename T, bool SAFE>
    class weak_ptr { // NOLINT: invalid case style
    public:
        using original = std::weak_ptr<T>;
        using element_type = typename original::element_type;

    private:
        template<class U, bool SAFE_P>
        friend class shared_ptr;

    private:
        original internal;

    public:
        // Constructors
        weak_ptr() : internal() {
        }

        // NOLINTBEGIN
        template<class U, typename std::enable_if<compatible_with_t<U, T>::value, int>::type = 0>
        weak_ptr(shared_ptr<U, SAFE> const &ptr) noexcept : internal(ptr.internal) {
        }

        weak_ptr(weak_ptr const &other) noexcept : internal(other.internal) {
        }

        template<class U, typename std::enable_if<compatible_with_t<U, T>::value, int>::type = 0>
        weak_ptr(weak_ptr<U> const &ptr) noexcept : internal(ptr.internal) {
        }
#ifdef GOOSE_CLANG_TIDY
        [[clang::reinitializes]]
#endif
        weak_ptr(weak_ptr &&ptr) noexcept
            : internal(std::move(ptr.internal)) {
        }

        template<class U, typename std::enable_if<compatible_with_t<U, T>::value, int>::type = 0>
#ifdef GOOSE_CLANG_TIDY
        [[clang::reinitializes]]
#endif
        weak_ptr(weak_ptr<U> &&ptr) noexcept
            : internal(std::move(ptr.internal)) {
        }

        // NOLINTEND
        // Destructor
        ~weak_ptr() = default;

        // Assignment operators
        weak_ptr &operator=(const weak_ptr &other) {
            if (this == &other) {
                return *this;
            }
            internal = other.internal;
            return *this;
        }

        template<class U, typename std::enable_if<compatible_with_t<U, T>::value, int>::type = 0>
        weak_ptr &operator=(const shared_ptr<U, SAFE> &ptr) {
            internal = ptr.internal;
            return *this;
        }

        // Modifiers
#ifdef GOOSE_CLANG_TIDY
        // This is necessary to tell clang-tidy that it reinitializes the variable after a move
        [[clang::reinitializes]]
#endif
        void
        reset() { // NOLINT: invalid case style
            internal.reset();
        }

        // Observers
        long use_count() const { // NOLINT: invalid case style
            return internal.use_count();
        }

        bool expired() const { // NOLINT: invalid case style
            return internal.expired();
        }

        shared_ptr<T, SAFE> lock() const { // NOLINT: invalid case style
            return shared_ptr<T, SAFE>(internal.lock());
        }

        // Relational operators
        template<typename U>
        bool operator==(const weak_ptr<U> &other) const noexcept {
            return internal == other.internal;
        }

        template<typename U>
        bool operator!=(const weak_ptr<U> &other) const noexcept {
            return internal != other.internal;
        }

        template<typename U>
        bool operator<(const weak_ptr<U> &other) const noexcept {
            return internal < other.internal;
        }

        template<typename U>
        bool operator<=(const weak_ptr<U> &other) const noexcept {
            return internal <= other.internal;
        }

        template<typename U>
        bool operator>(const weak_ptr<U> &other) const noexcept {
            return internal > other.internal;
        }

        template<typename U>
        bool operator>=(const weak_ptr<U> &other) const noexcept {
            return internal >= other.internal;
        }
    };

    template<typename T>
    using unsafe_weak_ptr = weak_ptr<T, false>;
} // namespace goose
