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

#include <goose/function/window/window_aggregator.h>

namespace goose {

class WindowAggregateExecutor;

// Used for validation
class WindowNaiveAggregator : public WindowAggregator {
public:
	WindowNaiveAggregator(const WindowAggregateExecutor &executor, WindowSharedExpressions &shared);
	~WindowNaiveAggregator() override;

	unique_ptr<LocalSinkState> GetLocalState(ExecutionContext &context, const GlobalSinkState &gstate) const override;
	void Evaluate(ExecutionContext &context, const DataChunk &bounds, Vector &result, idx_t count, idx_t row_idx,
	              OperatorSinkInput &sink) const override;

	static bool CanAggregate(const BoundWindowExpression &wexpr);

	//! The parent executor
	const WindowAggregateExecutor &executor;
	//! The column indices of any ORDER BY argument expressions
	vector<column_t> arg_order_idx;
};

} // namespace goose
