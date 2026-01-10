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

#include <goose/parser/base_expression.h>
#include <goose/common/vector.h>
#include <goose/parser/qualified_name.h>

namespace goose {
class Deserializer;
class Serializer;

//!  The ParsedExpression class is a base class that can represent any expression
//!  part of a SQL statement.
/*!
 The ParsedExpression class is a base class that can represent any expression
 part of a SQL statement. This is, for example, a column reference in a SELECT
 clause, but also operators, aggregates or filters. The Expression is emitted by the parser and does not contain any
 information about bindings to the catalog or to the types. ParsedExpressions are transformed into regular Expressions
 in the Binder.
 */
class ParsedExpression : public BaseExpression {
public:
	//! Create an Expression
	ParsedExpression(ExpressionType type, ExpressionClass expression_class) : BaseExpression(type, expression_class) {
	}

public:
	bool IsAggregate() const override;
	bool IsWindow() const override;
	bool HasSubquery() const override;
	bool IsScalar() const override;
	bool HasParameter() const override;

	bool Equals(const BaseExpression &other) const override;
	hash_t Hash() const override;

	//! Create a copy of this expression
	virtual unique_ptr<ParsedExpression> Copy() const = 0;

	virtual void Serialize(Serializer &serializer) const;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);

	static bool Equals(const unique_ptr<ParsedExpression> &left, const unique_ptr<ParsedExpression> &right);
	static bool ListEquals(const vector<unique_ptr<ParsedExpression>> &left,
	                       const vector<unique_ptr<ParsedExpression>> &right);

protected:
	//! Copy base Expression properties from another expression to this one,
	//! used in Copy method
	void CopyProperties(const ParsedExpression &other) {
		type = other.type;
		expression_class = other.expression_class;
		alias = other.alias;
		query_location = other.query_location;
	}
};

} // namespace goose
