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

//! The AT clause specifies which version of a table to read
class AtClause {
public:
	AtClause(string unit, unique_ptr<ParsedExpression> expr);

public:
	const string &Unit() {
		return unit;
	}
	unique_ptr<ParsedExpression> &ExpressionMutable() {
		return expr;
	}

	string ToString() const;
	bool Equals(const AtClause &other_p) const;
	unique_ptr<AtClause> Copy() const;
	void Serialize(Serializer &serializer) const;
	static unique_ptr<AtClause> Deserialize(Deserializer &source);

	static bool Equals(optional_ptr<AtClause> lhs, optional_ptr<AtClause> rhs);

private:
	//! The unit (e.g. TIMESTAMP or VERSION)
	string unit;
	//! The expression that determines which value of the unit we want to read
	unique_ptr<ParsedExpression> expr;
};

} // namespace goose
