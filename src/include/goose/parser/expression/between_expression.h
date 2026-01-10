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

namespace goose {

class BetweenExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BETWEEN;

public:
	GOOSE_API BetweenExpression(unique_ptr<ParsedExpression> input, unique_ptr<ParsedExpression> lower,
	                             unique_ptr<ParsedExpression> upper);

	unique_ptr<ParsedExpression> input;
	unique_ptr<ParsedExpression> lower;
	unique_ptr<ParsedExpression> upper;

public:
	string ToString() const override;

	static bool Equal(const BetweenExpression &a, const BetweenExpression &b);

	unique_ptr<ParsedExpression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);

public:
	template <class T, class BASE>
	static string ToString(const T &entry) {
		return "(" + entry.input->ToString() + " BETWEEN " + entry.lower->ToString() + " AND " +
		       entry.upper->ToString() + ")";
	}

private:
	BetweenExpression();
};
} // namespace goose
