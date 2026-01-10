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
#include <goose/common/types.h>

namespace goose {

//! CastExpression represents a type cast from one SQL type to another SQL type
class CastExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::CAST;

public:
	GOOSE_API CastExpression(LogicalType target, unique_ptr<ParsedExpression> child, bool try_cast = false);

	//! The child of the cast expression
	unique_ptr<ParsedExpression> child;
	//! The type to cast to
	LogicalType cast_type;
	//! Whether or not this is a try_cast expression
	bool try_cast;

public:
	string ToString() const override;

	static bool Equal(const CastExpression &a, const CastExpression &b);

	unique_ptr<ParsedExpression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);

public:
	template <class T, class BASE>
	static string ToString(const T &entry) {
		return (entry.try_cast ? "TRY_CAST(" : "CAST(") + entry.child->ToString() + " AS " +
		       entry.cast_type.ToString() + ")";
	}

private:
	CastExpression();
};
} // namespace goose
