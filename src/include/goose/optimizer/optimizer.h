// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
