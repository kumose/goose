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

#include <goose/common/enum_util.h>
#include <goose/optimizer/rule.h>

namespace goose {

// DateTruncSimplificationRule rewrites an expression of the form
//
//    date_trunc(part, column) <op> const_rhs
//
// such that the date_trunc is instead applied to the constant RHS and then simplified further.
// The rules applied are as follows:
//
//   date_trunc(part, column) >= const_rhs   -->   column >= date_trunc(part, date_add(const_rhs, INTERVAL 1 part))
//    - but if date_trunc(const_rhs) = const_rhs, then we can do column >= const_rhs
//
//   date_trunc(part, column) <= const_rhs   -->   column <= date_trunc(part, date_add(const_rhs, INTERVAL 1 part))
//
//   date_trunc(part, column) >  const_rhs   -->   column >= date_trunc(part, date_add(const_rhs, INTERVAL 1 part))
//    - note the change from > to >=!
//
//   date_trunc(part, column) <  const_rhs   -->   column <  date_trunc(part, date_add(const_rhs, INTERVAL 1 part))
//    - but if date_trunc(const_rhs) = const_rhs, then we can do column < const_rhs
//
//   date_trunc(part, column) == const_rhs   -->   column >= date_trunc(part, const_rhs) AND
//                                                    column <  date_trunc(part, date_add(const_rhs, INTERVAL 1 part))
//    - but if date_trunc(const_rhs) != const_rhs, then this one is unsatisfiable
//
//   date_trunc(part, column) <> const_rhs   -->   column <  date_trunc(part, const_rhs) OR
//                                                    column >= date_trunc(part, date_add(const_rhs, INTERVAL 1 part))
//    - but if date_trunc(const_rhs) != const_rhs, then this is always satisfied
//
//   date_trunc(part, column) IS NOT DISTINCT FROM const_rhs  --> (column >= date_trunc(part, const_rhs) AND
//                                                                 column < date_trunc(part,
//                                                                     date_add(const_rhs, INTERVAL 1 part)) AND
//                                                                 column IS NOT NULL)
//    - but if const_rhs is NULL, then this is just 'column IS NULL'
//
//   date_trunc(part, column) IS DISTINCT FROM const_rhs  -->  (column < date_trunc(part, const_rhs) OR
//                                                              column >= date_trunc(part,
//                                                                  date_add(const_rhs, INTERVAL 1 part)) OR
//                                                              column IS NULL)
//    - but if const_rhs is NULL, then this is just 'column IS NOT NULL'
//
class DateTruncSimplificationRule : public Rule {
public:
	explicit DateTruncSimplificationRule(ExpressionRewriter &rewriter);

	unique_ptr<Expression> Apply(LogicalOperator &op, vector<reference<Expression>> &bindings, bool &changes_made,
	                             bool is_root) override;

	static string DatePartToFunc(const DatePartSpecifier &date_part);

	unique_ptr<Expression> CreateTrunc(const BoundConstantExpression &date_part, const BoundConstantExpression &rhs,
	                                   const LogicalType &return_type);
	unique_ptr<Expression> CreateTruncAdd(const BoundConstantExpression &date_part, const BoundConstantExpression &rhs,
	                                      const LogicalType &return_type);

	bool DateIsTruncated(const BoundConstantExpression &date_part, const BoundConstantExpression &rhs);

	unique_ptr<Expression> CastAndEvaluate(unique_ptr<Expression> rhs, const LogicalType &return_type);
};

} // namespace goose
