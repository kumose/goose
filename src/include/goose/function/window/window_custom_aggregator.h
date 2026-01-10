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

#include <goose/function/window/window_aggregator.h>

namespace goose {

class WindowCustomAggregator : public WindowAggregator {
public:
	static bool CanAggregate(const BoundWindowExpression &wexpr, WindowAggregationMode mode);

	WindowCustomAggregator(const BoundWindowExpression &wexpr, WindowSharedExpressions &shared);
	~WindowCustomAggregator() override;

	unique_ptr<GlobalSinkState> GetGlobalState(ClientContext &context, idx_t group_count,
	                                           const ValidityMask &partition_mask) const override;
	void Finalize(ExecutionContext &context, CollectionPtr collection, const FrameStats &stats,
	              OperatorSinkInput &sink) override;

	unique_ptr<LocalSinkState> GetLocalState(ExecutionContext &context, const GlobalSinkState &gstate) const override;
	void Evaluate(ExecutionContext &context, const DataChunk &bounds, Vector &result, idx_t count, idx_t row_idx,
	              OperatorSinkInput &sink) const override;
};

} // namespace goose
