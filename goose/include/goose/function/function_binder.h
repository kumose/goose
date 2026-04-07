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

#include <goose/function/function.h>
#include <goose/function/cast/cast_function_set.h>
#include <goose/function/scalar_function.h>
#include <goose/function/aggregate_function.h>
#include <goose/function/function_set.h>
#include <goose/common/exception/binder_exception.h>
#include <goose/common/error_data.h>

namespace goose {

//! The FunctionBinder class is responsible for binding functions
class FunctionBinder {
public:
	GOOSE_API explicit FunctionBinder(Binder &binder);
	GOOSE_API explicit FunctionBinder(ClientContext &context);

	optional_ptr<Binder> binder;
	ClientContext &context;

public:
	//! Bind a scalar function from the set of functions and input arguments. Returns the index of the chosen function,
	//! returns optional_idx() and sets error if none could be found
	GOOSE_API optional_idx BindFunction(const string &name, ScalarFunctionSet &functions,
	                                     const vector<LogicalType> &arguments, ErrorData &error);
	GOOSE_API optional_idx BindFunction(const string &name, ScalarFunctionSet &functions,
	                                     vector<unique_ptr<Expression>> &arguments, ErrorData &error);
	//! Bind an aggregate function from the set of functions and input arguments. Returns the index of the chosen
	//! function, returns optional_idx() and sets error if none could be found
	GOOSE_API optional_idx BindFunction(const string &name, AggregateFunctionSet &functions,
	                                     const vector<LogicalType> &arguments, ErrorData &error);
	GOOSE_API optional_idx BindFunction(const string &name, AggregateFunctionSet &functions,
	                                     vector<unique_ptr<Expression>> &arguments, ErrorData &error);
	//! Bind a table function from the set of functions and input arguments. Returns the index of the chosen
	//! function, returns optional_idx() and sets error if none could be found
	GOOSE_API optional_idx BindFunction(const string &name, TableFunctionSet &functions,
	                                     const vector<LogicalType> &arguments, ErrorData &error);
	GOOSE_API optional_idx BindFunction(const string &name, TableFunctionSet &functions,
	                                     vector<unique_ptr<Expression>> &arguments, ErrorData &error);
	//! Bind a pragma function from the set of functions and input arguments
	GOOSE_API optional_idx BindFunction(const string &name, PragmaFunctionSet &functions, vector<Value> &parameters,
	                                     ErrorData &error);

	GOOSE_API unique_ptr<Expression> BindScalarFunction(const string &schema, const string &name,
	                                                     vector<unique_ptr<Expression>> children, ErrorData &error,
	                                                     bool is_operator = false,
	                                                     optional_ptr<Binder> binder = nullptr);
	GOOSE_API unique_ptr<Expression> BindScalarFunction(ScalarFunctionCatalogEntry &function,
	                                                     vector<unique_ptr<Expression>> children, ErrorData &error,
	                                                     bool is_operator = false,
	                                                     optional_ptr<Binder> binder = nullptr);

	GOOSE_API unique_ptr<Expression> BindScalarFunction(ScalarFunction bound_function,
	                                                     vector<unique_ptr<Expression>> children,
	                                                     bool is_operator = false,
	                                                     optional_ptr<Binder> binder = nullptr);

	GOOSE_API unique_ptr<BoundAggregateExpression>
	BindAggregateFunction(AggregateFunction bound_function, vector<unique_ptr<Expression>> children,
	                      unique_ptr<Expression> filter = nullptr,
	                      AggregateType aggr_type = AggregateType::NON_DISTINCT);

	GOOSE_API static void BindSortedAggregate(ClientContext &context, BoundAggregateExpression &expr,
	                                           const vector<unique_ptr<Expression>> &groups,
	                                           optional_ptr<vector<GroupingSet>> grouping_sets);
	GOOSE_API static void BindSortedAggregate(ClientContext &context, BoundWindowExpression &expr);

	//! Cast a set of expressions to the arguments of this function
	void CastToFunctionArguments(SimpleFunction &function, vector<unique_ptr<Expression>> &children);

	void ResolveTemplateTypes(BaseScalarFunction &bound_function, const vector<unique_ptr<Expression>> &children);
	void CheckTemplateTypesResolved(const BaseScalarFunction &bound_function);

private:
	optional_idx BindVarArgsFunctionCost(const SimpleFunction &func, const vector<LogicalType> &arguments);
	optional_idx BindFunctionCost(const SimpleFunction &func, const vector<LogicalType> &arguments);

	template <class T>
	vector<idx_t> BindFunctionsFromArguments(const string &name, FunctionSet<T> &functions,
	                                         const vector<LogicalType> &arguments, ErrorData &error);

	template <class T>
	optional_idx MultipleCandidateException(const string &catalog_name, const string &schema_name, const string &name,
	                                        FunctionSet<T> &functions, vector<idx_t> &candidate_functions,
	                                        const vector<LogicalType> &arguments, ErrorData &error);

	template <class T>
	optional_idx BindFunctionFromArguments(const string &name, FunctionSet<T> &functions,
	                                       const vector<LogicalType> &arguments, ErrorData &error);

	vector<LogicalType> GetLogicalTypesFromExpressions(vector<unique_ptr<Expression>> &arguments);
};

} // namespace goose
