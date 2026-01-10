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
