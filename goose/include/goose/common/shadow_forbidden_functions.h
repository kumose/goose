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

#include <goose/common/types-import.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/shared_ptr.h>

#ifndef GOOSE_CLANG_TIDY
namespace std {
    template<class C>
    bool isspace(C c) {
        static_assert(sizeof(C) == 0, "Use StringUtil::CharacterIsSpace instead of isspace!");
        return false;
    }
#ifndef GOOSE_ENABLE_DEPRECATED_API
    template<class T, class... ARGS>
    static std::unique_ptr<T> make_unique(ARGS &&... __args) { // NOLINT: mimic std style
        static_assert(sizeof(T) == 0, "Use make_uniq instead of make_unique!");
        return nullptr;
    }

    template<class T, class... ARGS>
    static std::shared_ptr<T> make_shared(ARGS &&... __args) { // NOLINT: mimic std style
        static_assert(sizeof(T) == 0, "Use make_shared_ptr instead of make_shared!");
        return nullptr;
    }
#endif // GOOSE_ENABLE_DEPRECATED_API

    template<class charT, class traits = char_traits<charT>, class Allocator = allocator<charT> >
    class basic_stringstream_mock;

    typedef basic_stringstream_mock<char> stringstream;
} // namespace std

using std::isspace;
using std::make_shared;
using std::make_unique;
#endif
