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
