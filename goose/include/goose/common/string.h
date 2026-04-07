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

#include <goose/common/types-import.h>
#include <string>
#include <locale>

namespace goose {
    using std::string;
} // namespace goose

namespace goose {
    template<class charT, class traits = std::char_traits<charT>, class Allocator = std::allocator<charT> >
    class basic_stringstream : public goose_base_std::basic_stringstream<charT, traits, Allocator> {
    public:
        using original = goose_base_std::basic_stringstream<charT, traits, Allocator>;

        explicit basic_stringstream(std::ios_base::openmode which = std::ios_base::out | std::ios_base::in)
            : original(which) {
            this->imbue(std::locale::classic());
        }

        explicit basic_stringstream(const std::basic_string<charT, traits, Allocator> &s,
                                    std::ios_base::openmode which = std::ios_base::out | std::ios_base::in)
            : original(s, which) {
            this->imbue(std::locale::classic());
        }

        basic_stringstream(const basic_stringstream &) = delete;

        basic_stringstream(basic_stringstream &&rhs) noexcept;
    };

    typedef basic_stringstream<char> stringstream;
} // namespace goose
