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

#include <goose/common/types/value.h>
#include <goose/common/types-import.h>
#include <goose/parser/expression_map.h>
#include <goose/planner/expression.h>
#include <goose/planner/filter/conjunction_filter.h>
#include <goose/planner/filter/constant_filter.h>

#include <goose/storage/data_table.h>
#include <functional>
#include <map>

namespace goose {
class Optimizer;

enum class ValueComparisonResult { PRUNE_LEFT, PRUNE_RIGHT, UNSATISFIABLE_CONDITION, PRUNE_NOTHING };
enum class FilterResult { UNSATISFIABLE, SUCCESS, UNSUPPORTED };
enum class FilterPushdownResult { NO_PUSHDOWN, PUSHED_DOWN_PARTIALLY, PUSHED_DOWN_FULLY };

//! The FilterCombiner combines several filters and generates a logically equivalent set that is more efficient
//! Amongst others:
//! (1) it prunes obsolete filter conditions: i.e. [X > 5 and X > 7] => [X > 7]
//! (2) it generates new filters for expressions in the same equivalence set: i.e. [X = Y and X = 500] => [Y = 500]
//! (3) it prunes branches that have unsatisfiable filters: i.e. [X = 5 AND X > 6] => FALSE, prune branch
class FilterCombiner {
public:
	explicit FilterCombiner(ClientContext &context);
	explicit FilterCombiner(Optimizer &optimizer);

	ClientContext &context;

public:
	struct ExpressionValueInformation {
		Value constant;
		ExpressionType comparison_type;
	};

	FilterResult AddFilter(unique_ptr<Expression> expr);

	//! Returns whether or not a set of integral values is a dense range (i.e. 1, 2, 3, 4, 5)
	//! If this returns true - this sorts "in_list" as a side-effect
	static bool IsDenseRange(vector<Value> &in_list);
	static bool ContainsNull(vector<Value> &in_list);
	static bool FindNextLegalUTF8(string &prefix_string);

	void GenerateFilters(const std::function<void(unique_ptr<Expression> filter)> &callback);
	bool HasFilters();
	TableFilterSet GenerateTableScanFilters(const vector<ColumnIndex> &column_ids,
	                                        vector<FilterPushdownResult> &pushdown_results);

	FilterPushdownResult TryPushdownGenericExpression(LogicalGet &get, Expression &expr);

private:
	FilterResult AddFilter(Expression &expr);
	FilterResult AddBoundComparisonFilter(Expression &expr);
	FilterResult AddTransitiveFilters(BoundComparisonExpression &comparison, bool is_root = true);
	unique_ptr<Expression> FindTransitiveFilter(Expression &expr);
	Expression &GetNode(Expression &expr);
	idx_t GetEquivalenceSet(Expression &expr);
	FilterResult AddConstantComparison(vector<ExpressionValueInformation> &info_list, ExpressionValueInformation info);

	FilterPushdownResult TryPushdownConstantFilter(TableFilterSet &table_filters, const vector<ColumnIndex> &column_ids,
	                                               column_t column_id, vector<ExpressionValueInformation> &info_list);
	FilterPushdownResult TryPushdownExpression(TableFilterSet &table_filters, const vector<ColumnIndex> &column_ids,
	                                           Expression &expr);
	FilterPushdownResult TryPushdownPrefixFilter(TableFilterSet &table_filters, const vector<ColumnIndex> &column_ids,
	                                             Expression &expr);
	FilterPushdownResult TryPushdownLikeFilter(TableFilterSet &table_filters, const vector<ColumnIndex> &column_ids,
	                                           Expression &expr);
	FilterPushdownResult TryPushdownInFilter(TableFilterSet &table_filters, const vector<ColumnIndex> &column_ids,
	                                         Expression &expr);
	FilterPushdownResult TryPushdownOrClause(TableFilterSet &table_filters, const vector<ColumnIndex> &column_ids,
	                                         Expression &expr);

private:
	vector<unique_ptr<Expression>> remaining_filters;

	expression_map_t<unique_ptr<Expression>> stored_expressions;
	expression_map_t<idx_t> equivalence_set_map;
	map<idx_t, vector<ExpressionValueInformation>> constant_values;
	map<idx_t, vector<reference<Expression>>> equivalence_map;
	idx_t set_index = 0;
};

} // namespace goose
