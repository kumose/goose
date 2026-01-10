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
#include <goose/common/enums/window_aggregation_mode.h>
#include <goose/function/window/window_aggregator.h>

namespace goose {

class WindowAggregateExecutor : public WindowExecutor {
public:
	WindowAggregateExecutor(BoundWindowExpression &wexpr, ClientContext &client, WindowSharedExpressions &shared,
	                        WindowAggregationMode mode);

	void Sink(ExecutionContext &context, DataChunk &sink_chunk, DataChunk &coll_chunk, const idx_t input_idx,
	          OperatorSinkInput &sink) const override;
	void Finalize(ExecutionContext &context, CollectionPtr collection, OperatorSinkInput &sink) const override;

	unique_ptr<GlobalSinkState> GetGlobalState(ClientContext &client, const idx_t payload_count,
	                                           const ValidityMask &partition_mask,
	                                           const ValidityMask &order_mask) const override;
	unique_ptr<LocalSinkState> GetLocalState(ExecutionContext &context, const GlobalSinkState &gstate) const override;

	const WindowAggregationMode mode;

	// aggregate computation algorithm
	unique_ptr<WindowAggregator> aggregator;

	// FILTER reference expression in sink_chunk
	unique_ptr<Expression> filter_ref;

protected:
	void EvaluateInternal(ExecutionContext &context, DataChunk &eval_chunk, Vector &result, idx_t count, idx_t row_idx,
	                      OperatorSinkInput &sink) const override;
};

} // namespace goose
