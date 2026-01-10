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
#include <goose/common/types.h>

namespace goose {
class BaseStatistics;
class ClientContext;

//!  The Expression class represents a bound Expression with a return type
class Expression : public BaseExpression {
public:
	Expression(ExpressionType type, ExpressionClass expression_class, LogicalType return_type);
	~Expression() override;

	//! The return type of the expression
	LogicalType return_type;
	//! Expression statistics (if any) - ONLY USED FOR VERIFICATION
	unique_ptr<BaseStatistics> verification_stats;

public:
	bool IsAggregate() const override;
	bool IsWindow() const override;
	bool HasSubquery() const override;
	bool IsScalar() const override;
	bool HasParameter() const override;

	virtual bool IsVolatile() const;
	virtual bool IsConsistent() const;
	virtual bool PropagatesNullValues() const;
	virtual bool IsFoldable() const;
	virtual bool CanThrow() const;

	hash_t Hash() const override;

	bool Equals(const BaseExpression &other) const override {
		if (!BaseExpression::Equals(other)) {
			return false;
		}
		return return_type == reinterpret_cast<const Expression &>(other).return_type;
	}
	static bool Equals(const Expression &left, const Expression &right) {
		return left.Equals(right);
	}
	static bool Equals(const unique_ptr<Expression> &left, const unique_ptr<Expression> &right);
	static bool ListEquals(const vector<unique_ptr<Expression>> &left, const vector<unique_ptr<Expression>> &right);
	//! Create a copy of this expression
	virtual unique_ptr<Expression> Copy() const = 0;

	virtual void Serialize(Serializer &serializer) const;
	static unique_ptr<Expression> Deserialize(Deserializer &deserializer);

protected:
	//! Copy base Expression properties from another expression to this one,
	//! used in Copy method
	void CopyProperties(const Expression &other) {
		type = other.type;
		expression_class = other.expression_class;
		alias = other.alias;
		return_type = other.return_type;
		query_location = other.query_location;
	}
};

} // namespace goose
