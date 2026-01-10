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

#include <goose/function/window/window_executor.h>

namespace goose {

// Base class for non-aggregate functions that have a payload
class WindowValueExecutor : public WindowExecutor {
public:
	WindowValueExecutor(BoundWindowExpression &wexpr, WindowSharedExpressions &shared);

	void Finalize(ExecutionContext &context, CollectionPtr collection, OperatorSinkInput &sink) const override;

	unique_ptr<GlobalSinkState> GetGlobalState(ClientContext &client, const idx_t payload_count,
	                                           const ValidityMask &partition_mask,
	                                           const ValidityMask &order_mask) const override;
	unique_ptr<LocalSinkState> GetLocalState(ExecutionContext &context, const GlobalSinkState &gstate) const override;

	//! The column index of the value column
	column_t child_idx = DConstants::INVALID_INDEX;
	//! The column index of the Nth column
	column_t nth_idx = DConstants::INVALID_INDEX;
	//! The column index of the offset column
	column_t offset_idx = DConstants::INVALID_INDEX;
	//! The column index of the default value column
	column_t default_idx = DConstants::INVALID_INDEX;
	//! The column indices of any ORDER BY argument expressions
	vector<column_t> arg_order_idx;
};

class WindowLeadLagExecutor : public WindowValueExecutor {
public:
	WindowLeadLagExecutor(BoundWindowExpression &wexpr, WindowSharedExpressions &shared);

	unique_ptr<GlobalSinkState> GetGlobalState(ClientContext &client, const idx_t payload_count,
	                                           const ValidityMask &partition_mask,
	                                           const ValidityMask &order_mask) const override;
	unique_ptr<LocalSinkState> GetLocalState(ExecutionContext &context, const GlobalSinkState &gstate) const override;

protected:
	void EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk, Vector &result, idx_t count, idx_t row_idx,
	                      OperatorSinkInput &sink) const override;
};

class WindowFirstValueExecutor : public WindowValueExecutor {
public:
	WindowFirstValueExecutor(BoundWindowExpression &wexpr, WindowSharedExpressions &shared);

protected:
	void EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk, Vector &result, idx_t count, idx_t row_idx,
	                      OperatorSinkInput &sink) const override;
};

class WindowLastValueExecutor : public WindowValueExecutor {
public:
	WindowLastValueExecutor(BoundWindowExpression &wexpr, WindowSharedExpressions &shared);

protected:
	void EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk, Vector &result, idx_t count, idx_t row_idx,
	                      OperatorSinkInput &sink) const override;
};

class WindowNthValueExecutor : public WindowValueExecutor {
public:
	WindowNthValueExecutor(BoundWindowExpression &wexpr, WindowSharedExpressions &shared);

protected:
	void EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk, Vector &result, idx_t count, idx_t row_idx,
	                      OperatorSinkInput &sink) const override;
};

class WindowFillExecutor : public WindowValueExecutor {
public:
	WindowFillExecutor(BoundWindowExpression &wexpr, WindowSharedExpressions &shared);

	//! Never ignore nulls (that's the point!)
	bool IgnoreNulls() const override {
		return false;
	}

	unique_ptr<GlobalSinkState> GetGlobalState(ClientContext &client, const idx_t payload_count,
	                                           const ValidityMask &partition_mask,
	                                           const ValidityMask &order_mask) const override;
	unique_ptr<LocalSinkState> GetLocalState(ExecutionContext &context, const GlobalSinkState &gstate) const override;

	//! Secondary order collection index
	idx_t order_idx = DConstants::INVALID_INDEX;

protected:
	void EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk, Vector &result, idx_t count, idx_t row_idx,
	                      OperatorSinkInput &sink) const override;
};

} // namespace goose
