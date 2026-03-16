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

#include <goose/common/common.h>
#include <goose/function/create_sort_key.h>

namespace goose {

struct AggregateSortKeyHelpers {
	template <class STATE, class OP, OrderType ORDER_TYPE = OrderType::ASCENDING, bool IGNORE_NULLS = true>
	static void UnaryUpdate(Vector inputs[], AggregateInputData &input_data, idx_t input_count, Vector &state_vector,
	                        idx_t count) {
		D_ASSERT(input_count == 1);
		auto &input = inputs[0];

		Vector sort_key(LogicalType::BLOB);
		auto modifiers = OrderModifiers(ORDER_TYPE, OrderByNullType::NULLS_LAST);
		CreateSortKeyHelpers::CreateSortKey(input, count, modifiers, sort_key);

		UnifiedVectorFormat idata;
		if (IGNORE_NULLS) {
			input.ToUnifiedFormat(count, idata);
		}

		UnifiedVectorFormat kdata;
		sort_key.ToUnifiedFormat(count, kdata);

		UnifiedVectorFormat sdata;
		state_vector.ToUnifiedFormat(count, sdata);

		auto key_data = UnifiedVectorFormat::GetData<string_t>(kdata);
		auto states = UnifiedVectorFormat::GetData<STATE *>(sdata);
		for (idx_t i = 0; i < count; i++) {
			const auto sidx = sdata.sel->get_index(i);
			if (IGNORE_NULLS) {
				auto idx = idata.sel->get_index(i);
				if (!idata.validity.RowIsValid(idx)) {
					continue;
				}
			}
			const auto key_idx = kdata.sel->get_index(i);
			auto &state = *states[sidx];
			OP::template Execute<string_t, STATE, OP>(state, key_data[key_idx], input_data);
		}
	}
};

} // namespace goose
