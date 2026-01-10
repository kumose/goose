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

#include <array>

#include <goose/common/types.h>
#include <goose/common/types/string_type.h>

namespace goose {
class RandomEngine;

//! The BaseUUID class contains UUID related common and util functions.
class BaseUUID {
public:
	constexpr static const uint8_t STRING_SIZE = 36;
	//! Convert a uuid string to a hugeint object
	static bool FromString(const string &str, hugeint_t &result, bool strict = false);
	//! Convert a uuid string to a hugeint object
	static bool FromCString(const char *str, idx_t len, hugeint_t &result) {
		return FromString(string(str, 0, len), result);
	}
	//! Convert a hugeint object to a uuid style string
	static void ToString(hugeint_t input, char *buf);

	//! Convert a uhugeint_t object to a uuid value
	static hugeint_t FromUHugeint(uhugeint_t input);
	//! Convert a uuid value to a uhugeint_t object
	static uhugeint_t ToUHugeint(hugeint_t input);

	//! Convert 16-byte binary data to UUID (hugeint_t)
	static hugeint_t FromBlob(const_data_ptr_t input);
	//! Convert UUID (hugeint_t) to 16-byte binary data
	static void ToBlob(hugeint_t input, data_ptr_t output);

	//! Convert a hugeint object to a uuid style string
	static string ToString(hugeint_t input) {
		char buff[STRING_SIZE];
		ToString(input, buff);
		return string(buff, STRING_SIZE);
	}

	static hugeint_t FromString(const string &str) {
		hugeint_t result;
		FromString(str, result);
		return result;
	}

protected:
	//! Util function, which converts uint8_t array to hugeint_t.
	static hugeint_t Convert(const std::array<uint8_t, 16> &bytes);
};

//! The UUIDv4 class contains static operations for the UUIDv4 type
class UUID : public BaseUUID {
public:
	//! Generate a random UUID v4 value.
	static hugeint_t GenerateRandomUUID(RandomEngine &engine);
	static hugeint_t GenerateRandomUUID();
};

using UUIDv4 = UUID;

//! The UUIDv7 class contains static operations for the UUIDv7 type.
class UUIDv7 : public BaseUUID {
public:
	//! Generate a random UUID v7 value.
	static hugeint_t GenerateRandomUUID(RandomEngine &engine);
	static hugeint_t GenerateRandomUUID();
};

} // namespace goose
