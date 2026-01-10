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

#include <goose/common/types-import.h>
#include <goose/planner/expression.h>

namespace goose {

//! JoinCondition represents a left-right comparison join condition
struct JoinCondition {
public:
	JoinCondition() {
	}

	//! Turns the JoinCondition into an expression; note that this destroys the JoinCondition as the expression inherits
	//! the left/right expressions
	static unique_ptr<Expression> CreateExpression(JoinCondition cond);
	static unique_ptr<Expression> CreateExpression(vector<JoinCondition> conditions);

	void Serialize(Serializer &serializer) const;
	static JoinCondition Deserialize(Deserializer &deserializer);

public:
	unique_ptr<Expression> left;
	unique_ptr<Expression> right;
	ExpressionType comparison;
};

class JoinSide {
public:
	enum JoinValue : uint8_t { NONE, LEFT, RIGHT, BOTH };

	JoinSide() = default;
	constexpr JoinSide(JoinValue val) : value(val) { // NOLINT: Allow implicit conversion from `join_value`
	}

	bool operator==(JoinSide a) const {
		return value == a.value;
	}
	bool operator!=(JoinSide a) const {
		return value != a.value;
	}

	static JoinSide CombineJoinSide(JoinSide left, JoinSide right);
	static JoinSide GetJoinSide(idx_t table_binding, const unordered_set<idx_t> &left_bindings,
	                            const unordered_set<uint64_t> &right_bindings);
	static JoinSide GetJoinSide(Expression &expression, const unordered_set<idx_t> &left_bindings,
	                            const unordered_set<idx_t> &right_bindings);
	static JoinSide GetJoinSide(const unordered_set<idx_t> &bindings, const unordered_set<idx_t> &left_bindings,
	                            const unordered_set<idx_t> &right_bindings);

private:
	JoinValue value;
};

} // namespace goose
