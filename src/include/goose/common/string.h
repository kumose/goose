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

#include <goose/common/types-import.h>
#include <string>
#include <locale>

namespace goose {
using std::string;
} // namespace goose

namespace goose {

template <class charT, class traits = std::char_traits<charT>, class Allocator = std::allocator<charT>>
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
