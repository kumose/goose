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

#include <goose/goose-c.h>
#include <goose/common/fast_float/fast_float.h>
#include <goose/common/string_util.h>

namespace goose {
template <class T>
static bool TryDoubleCast(const char *buf, idx_t len, T &result, bool strict, char decimal_separator = '.') {
	// skip any spaces at the start
	while (len > 0 && StringUtil::CharacterIsSpace(*buf)) {
		buf++;
		len--;
	}
	if (len == 0) {
		return false;
	}
	if (*buf == '+') {
		if (strict) {
			// plus is not allowed in strict mode
			return false;
		}
		buf++;
		len--;
	}
	if (strict && len >= 2) {
		if (buf[0] == '0' && StringUtil::CharacterIsDigit(buf[1])) {
			// leading zeros are not allowed in strict mode
			return false;
		}
	}
	auto endptr = buf + len;
	auto parse_result = goose_fast_float::from_chars(buf, buf + len, result, strict, decimal_separator);
	if (parse_result.ec != std::errc()) {
		return false;
	}
	auto current_end = parse_result.ptr;
	if (!strict) {
		while (current_end < endptr && StringUtil::CharacterIsSpace(*current_end)) {
			current_end++;
		}
	}
	return current_end == endptr;
}
} // namespace goose
