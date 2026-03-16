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
