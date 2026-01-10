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

#include <goose/main/client_context.h>
#include <goose/optimizer/column_binding_replacer.h>
#include <goose/optimizer/remove_unused_columns.h>

namespace goose {

enum class TopNPayloadType { SINGLE_COLUMN, STRUCT_PACK };

struct TopNWindowEliminationParameters {
	//! Whether the sort is ASCENDING or DESCENDING
	OrderType order_type;
	//! The number of values in the LIMIT clause
	int64_t limit;
	//! How we fetch the payload columns
	TopNPayloadType payload_type;
	//! Whether to include row numbers
	bool include_row_number;
	//! Whether the val or arg column contains null values
	bool can_be_null = false;
};

class TopNWindowElimination : public BaseColumnPruner {
public:
	explicit TopNWindowElimination(ClientContext &context, Optimizer &optimizer,
	                               optional_ptr<column_binding_map_t<unique_ptr<BaseStatistics>>> stats_p);

	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);

private:
	bool CanOptimize(LogicalOperator &op);
	unique_ptr<LogicalOperator> OptimizeInternal(unique_ptr<LogicalOperator> op, ColumnBindingReplacer &replacer);

	unique_ptr<LogicalOperator> CreateAggregateOperator(LogicalWindow &window, vector<unique_ptr<Expression>> args,
	                                                    const TopNWindowEliminationParameters &params) const;
	unique_ptr<LogicalOperator> TryCreateUnnestOperator(unique_ptr<LogicalOperator> op,
	                                                    const TopNWindowEliminationParameters &params) const;
	unique_ptr<LogicalOperator> CreateProjectionOperator(unique_ptr<LogicalOperator> op,
	                                                     const TopNWindowEliminationParameters &params,
	                                                     const map<idx_t, idx_t> &group_idxs) const;

	vector<unique_ptr<Expression>> GenerateAggregatePayload(const vector<ColumnBinding> &bindings,
	                                                        const LogicalWindow &window, map<idx_t, idx_t> &group_idxs);
	vector<ColumnBinding> TraverseProjectionBindings(const std::vector<ColumnBinding> &old_bindings,
	                                                 reference<LogicalOperator> &op);
	unique_ptr<Expression> CreateAggregateExpression(vector<unique_ptr<Expression>> aggregate_params, bool requires_arg,
	                                                 const TopNWindowEliminationParameters &params) const;
	unique_ptr<Expression> CreateRowNumberGenerator(unique_ptr<Expression> aggregate_column_ref) const;
	void AddStructExtractExprs(vector<unique_ptr<Expression>> &exprs, const LogicalType &struct_type,
	                           const unique_ptr<BoundColumnRefExpression> &aggregate_column_ref) const;
	static void UpdateTopmostBindings(idx_t window_idx, const unique_ptr<LogicalOperator> &op,
	                                  const map<idx_t, idx_t> &group_idxs,
	                                  const vector<ColumnBinding> &topmost_bindings,
	                                  vector<ColumnBinding> &new_bindings, ColumnBindingReplacer &replacer);
	TopNWindowEliminationParameters ExtractOptimizerParameters(const LogicalWindow &window, const LogicalFilter &filter,
	                                                           const vector<ColumnBinding> &bindings,
	                                                           vector<unique_ptr<Expression>> &aggregate_payload);

	// Semi-join reduction methods
	unique_ptr<LogicalOperator> TryPrepareLateMaterialization(const LogicalWindow &window,
	                                                          vector<unique_ptr<Expression>> &args);
	unique_ptr<LogicalOperator> ConstructLHS(LogicalGet &rhs, vector<idx_t> &projections) const;
	static unique_ptr<LogicalOperator> ConstructJoin(unique_ptr<LogicalOperator> lhs, unique_ptr<LogicalOperator> rhs,
	                                                 idx_t rhs_rowid_idx,
	                                                 const TopNWindowEliminationParameters &params);
	bool CanUseLateMaterialization(const LogicalWindow &window, vector<unique_ptr<Expression>> &args,
	                               vector<idx_t> &projections, vector<reference<LogicalOperator>> &stack);

private:
	ClientContext &context;
	Optimizer &optimizer;
	optional_ptr<column_binding_map_t<unique_ptr<BaseStatistics>>> stats;
};
} // namespace goose
