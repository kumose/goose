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

#include <goose/optimizer/expression_rewriter.h>
#include <goose/planner/logical_operator.h>
#include <goose/planner/logical_operator_visitor.h>
#include <goose/common/enums/optimizer_type.h>

#include <functional>

namespace goose {
class Binder;

class Optimizer {
public:
	Optimizer(Binder &binder, ClientContext &context);

	//! Optimize a plan by running specialized optimizers
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> plan);
	//! Return a reference to the client context of this optimizer
	ClientContext &GetContext();
	//! Whether the specific optimizer is disabled
	bool OptimizerDisabled(OptimizerType type);
	static bool OptimizerDisabled(ClientContext &context, OptimizerType type);

public:
	ClientContext &context;
	Binder &binder;
	ExpressionRewriter rewriter;

private:
	void RunBuiltInOptimizers();
	void RunOptimizer(OptimizerType type, const std::function<void()> &callback);
	void Verify(LogicalOperator &op);

public:
	// helper functions
	unique_ptr<Expression> BindScalarFunction(const string &name, unique_ptr<Expression> c1);
	unique_ptr<Expression> BindScalarFunction(const string &name, unique_ptr<Expression> c1, unique_ptr<Expression> c2);

private:
	unique_ptr<LogicalOperator> plan;

private:
	unique_ptr<Expression> BindScalarFunction(const string &name, vector<unique_ptr<Expression>> children);
};

} // namespace goose
