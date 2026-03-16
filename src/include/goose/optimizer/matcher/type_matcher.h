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

#include <goose/common/types.h>

namespace goose {

//! The TypeMatcher class contains a set of matchers that can be used to pattern match TypeIds for Rules
class TypeMatcher {
public:
	virtual ~TypeMatcher() {
	}

	virtual bool Match(const LogicalType &type) = 0;
};

//! The SpecificTypeMatcher class matches only a single specified type
class SpecificTypeMatcher : public TypeMatcher {
public:
	explicit SpecificTypeMatcher(LogicalType type) : type(std::move(type)) {
	}

	bool Match(const LogicalType &type_p) override {
		return type_p == this->type;
	}

private:
	LogicalType type;
};
//! The SpecificTypeMatcher class matches only a type out of a set of types
class SetTypesMatcher : public TypeMatcher {
public:
	explicit SetTypesMatcher(vector<LogicalType> types_p) : types(std::move(types_p)) {
	}

	bool Match(const LogicalType &type_p) override {
		for (auto &type : types) {
			if (type == type_p) {
				return true;
			}
		}
		return false;
	}

private:
	vector<LogicalType> types;
};

//! The NumericTypeMatcher class matches any numeric type (DECIMAL, INTEGER, etc...)
class NumericTypeMatcher : public TypeMatcher {
public:
	bool Match(const LogicalType &type) override {
		return type.IsNumeric();
	}
};

//! The IntegerTypeMatcher class matches only integer types (INTEGER, SMALLINT, TINYINT, BIGINT)
class IntegerTypeMatcher : public TypeMatcher {
public:
	bool Match(const LogicalType &type) override {
		return type.IsIntegral();
	}
};

} // namespace goose
