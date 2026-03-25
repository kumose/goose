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
