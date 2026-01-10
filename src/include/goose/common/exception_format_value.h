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

#include <goose/common/string.h>
#include <goose/common/hugeint.h>

#include <vector>

namespace goose {

class String;

// Helper class to support custom overloading
// Escaping " and quoting the value with "
class SQLIdentifier {
public:
	explicit SQLIdentifier(const string &raw_string) : raw_string(raw_string) {
	}

public:
	string raw_string;
};

// Helper class to support custom overloading
// Escaping ' and quoting the value with '
class SQLString {
public:
	explicit SQLString(const string &raw_string) : raw_string(raw_string) {
	}

public:
	string raw_string;
};

enum class PhysicalType : uint8_t;
struct LogicalType;

enum class ExceptionFormatValueType : uint8_t {
	FORMAT_VALUE_TYPE_DOUBLE,
	FORMAT_VALUE_TYPE_INTEGER,
	FORMAT_VALUE_TYPE_STRING
};

struct ExceptionFormatValue {
	GOOSE_API ExceptionFormatValue(double dbl_val);        // NOLINT
	GOOSE_API ExceptionFormatValue(int64_t int_val);       // NOLINT
	GOOSE_API ExceptionFormatValue(idx_t uint_val);        // NOLINT
	GOOSE_API ExceptionFormatValue(string str_val);        // NOLINT
	GOOSE_API ExceptionFormatValue(const String &str_val); // NOLINT
	GOOSE_API ExceptionFormatValue(hugeint_t hg_val);      // NOLINT
	GOOSE_API ExceptionFormatValue(uhugeint_t uhg_val);    // NOLINT

	ExceptionFormatValueType type;

	double dbl_val = 0;
	hugeint_t int_val = 0;
	string str_val;

public:
	template <class T>
	static ExceptionFormatValue CreateFormatValue(const T &value) {
		return int64_t(value);
	}
	static string Format(const string &msg, std::vector<ExceptionFormatValue> &values);
};

template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const PhysicalType &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const SQLString &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const SQLIdentifier &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const LogicalType &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const float &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const double &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const string &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const String &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const char *const &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(char *const &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const idx_t &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const hugeint_t &value);
template <>
GOOSE_API ExceptionFormatValue ExceptionFormatValue::CreateFormatValue(const uhugeint_t &value);

} // namespace goose
