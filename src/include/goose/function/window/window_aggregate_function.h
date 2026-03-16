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
