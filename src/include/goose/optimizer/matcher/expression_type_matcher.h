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

#include <goose/common/common.h>
#include <goose/common/enums/expression_type.h>
#include <goose/common/vector.h>

#include <algorithm>

namespace goose {

//! The ExpressionTypeMatcher class contains a set of matchers that can be used to pattern match ExpressionTypes
class ExpressionTypeMatcher {
public:
	virtual ~ExpressionTypeMatcher() {
	}

	virtual bool Match(ExpressionType type) = 0;
};

//! The SpecificExpressionTypeMatcher class matches a single specified Expression type
class SpecificExpressionTypeMatcher : public ExpressionTypeMatcher {
public:
	explicit SpecificExpressionTypeMatcher(ExpressionType type) : type(type) {
	}

	bool Match(ExpressionType type) override {
		return type == this->type;
	}

private:
	ExpressionType type;
};

//! The ManyExpressionTypeMatcher class matches a set of ExpressionTypes
class ManyExpressionTypeMatcher : public ExpressionTypeMatcher {
public:
	explicit ManyExpressionTypeMatcher(vector<ExpressionType> types) : types(std::move(types)) {
	}

	bool Match(ExpressionType type) override {
		return std::find(types.begin(), types.end(), type) != types.end();
	}

private:
	vector<ExpressionType> types;
};

//! The ComparisonExpressionTypeMatcher class matches a comparison expression
class ComparisonExpressionTypeMatcher : public ExpressionTypeMatcher {
public:
	bool Match(ExpressionType type) override {
		return type == ExpressionType::COMPARE_EQUAL || type == ExpressionType::COMPARE_GREATERTHANOREQUALTO ||
		       type == ExpressionType::COMPARE_LESSTHANOREQUALTO || type == ExpressionType::COMPARE_LESSTHAN ||
		       type == ExpressionType::COMPARE_GREATERTHAN;
	}
};
} // namespace goose
