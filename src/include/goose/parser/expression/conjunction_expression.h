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

#include <goose/parser/parsed_expression.h>
#include <goose/common/vector.h>

namespace goose {

//! Represents a conjunction (AND/OR)
class ConjunctionExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::CONJUNCTION;

public:
	GOOSE_API explicit ConjunctionExpression(ExpressionType type);
	GOOSE_API ConjunctionExpression(ExpressionType type, vector<unique_ptr<ParsedExpression>> children);
	GOOSE_API ConjunctionExpression(ExpressionType type, unique_ptr<ParsedExpression> left,
	                                 unique_ptr<ParsedExpression> right);

	vector<unique_ptr<ParsedExpression>> children;

public:
	void AddExpression(unique_ptr<ParsedExpression> expr);

	string ToString() const override;

	static bool Equal(const ConjunctionExpression &a, const ConjunctionExpression &b);

	unique_ptr<ParsedExpression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);

public:
	template <class T, class BASE>
	static string ToString(const T &entry) {
		string result = "(" + entry.children[0]->ToString();
		for (idx_t i = 1; i < entry.children.size(); i++) {
			result += " " + ExpressionTypeToOperator(entry.GetExpressionType()) + " " + entry.children[i]->ToString();
		}
		return result + ")";
	}
};
} // namespace goose
