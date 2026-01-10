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

#include <goose/planner/column_binding.h>
#include <goose/planner/expression.h>

namespace goose {

//! A BoundColumnRef expression represents a ColumnRef expression that was bound to an actual table and column index. It
//! is not yet executable, however. The ColumnBindingResolver transforms the BoundColumnRefExpressions into
//! BoundExpressions, which refer to indexes into the physical chunks that pass through the executor.
class BoundColumnRefExpression : public Expression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BOUND_COLUMN_REF;

public:
	BoundColumnRefExpression(LogicalType type, ColumnBinding binding, idx_t depth = 0);
	BoundColumnRefExpression(string alias, LogicalType type, ColumnBinding binding, idx_t depth = 0);

	//! Column index set by the binder, used to generate the final BoundExpression
	ColumnBinding binding;
	//! The subquery depth (i.e. depth 0 = current query, depth 1 = parent query, depth 2 = parent of parent, etc...).
	//! This is only non-zero for correlated expressions inside subqueries.
	idx_t depth;

public:
	bool IsScalar() const override {
		return false;
	}
	bool IsFoldable() const override {
		return false;
	}

	string ToString() const override;
	string GetName() const override;

	bool Equals(const BaseExpression &other) const override;
	hash_t Hash() const override;

	unique_ptr<Expression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<Expression> Deserialize(Deserializer &deserializer);
};
} // namespace goose
