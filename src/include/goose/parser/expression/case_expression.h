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

struct CaseCheck {
	unique_ptr<ParsedExpression> when_expr;
	unique_ptr<ParsedExpression> then_expr;

	void Serialize(Serializer &serializer) const;
	static CaseCheck Deserialize(Deserializer &deserializer);
};

//! The CaseExpression represents a CASE expression in the query
class CaseExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::CASE;

public:
	GOOSE_API CaseExpression();

	vector<CaseCheck> case_checks;
	unique_ptr<ParsedExpression> else_expr;

public:
	string ToString() const override;

	static bool Equal(const CaseExpression &a, const CaseExpression &b);

	unique_ptr<ParsedExpression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);

public:
	template <class T, class BASE>
	static string ToString(const T &entry) {
		string case_str = "CASE ";
		for (auto &check : entry.case_checks) {
			case_str += " WHEN (" + check.when_expr->ToString() + ")";
			case_str += " THEN (" + check.then_expr->ToString() + ")";
		}
		case_str += " ELSE " + entry.else_expr->ToString();
		case_str += " END";
		return case_str;
	}
};
} // namespace goose
