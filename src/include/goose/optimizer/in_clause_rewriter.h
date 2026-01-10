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

#include <goose/planner/logical_operator_visitor.h>
#include <goose/planner/logical_operator.h>
#include <goose/common/optional_ptr.h>

namespace goose {
class ClientContext;
class Optimizer;

class InClauseRewriter : public LogicalOperatorVisitor {
public:
	explicit InClauseRewriter(ClientContext &context, Optimizer &optimizer) : context(context), optimizer(optimizer) {
	}

	ClientContext &context;
	Optimizer &optimizer;
	optional_ptr<LogicalOperator> current_op;
	unique_ptr<LogicalOperator> root;

public:
	unique_ptr<LogicalOperator> Rewrite(unique_ptr<LogicalOperator> op);

	unique_ptr<Expression> VisitReplace(BoundOperatorExpression &expr, unique_ptr<Expression> *expr_ptr) override;
};

} // namespace goose
