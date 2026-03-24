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

struct HistogramFunctor {
	template <class T>
	static void HistogramFinalize(T value, Vector &result, idx_t offset) {
		FlatVector::GetData<T>(result)[offset] = value;
	}

	static bool CreateExtraState(idx_t count) {
		return false;
	}

	static void PrepareData(Vector &input, idx_t count, bool &, UnifiedVectorFormat &result) {
		input.ToUnifiedFormat(count, result);
	}

	template <class T>
	static T ExtractValue(UnifiedVectorFormat &bin_data, idx_t offset, AggregateInputData &) {
		return UnifiedVectorFormat::GetData<T>(bin_data)[bin_data.sel->get_index(offset)];
	}

	static bool RequiresExtract() {
		return false;
	}
};

struct HistogramStringFunctorBase {
	template <class T>
	static T ExtractValue(UnifiedVectorFormat &bin_data, idx_t offset, AggregateInputData &aggr_input) {
		auto &input_str = UnifiedVectorFormat::GetData<T>(bin_data)[bin_data.sel->get_index(offset)];
		if (input_str.IsInlined()) {
			// inlined strings can be inserted directly
			return input_str;
		}
		// if the string is not inlined we need to allocate space for it
		auto input_str_size = UnsafeNumericCast<uint32_t>(input_str.GetSize());
		auto string_memory = aggr_input.allocator.Allocate(input_str_size);
		// copy over the string
		memcpy(string_memory, input_str.GetData(), input_str_size);
		// now insert it into the histogram
		string_t histogram_str(char_ptr_cast(string_memory), input_str_size);
		return histogram_str;
	}

	static bool RequiresExtract() {
		return true;
	}
};

struct HistogramStringFunctor : HistogramStringFunctorBase {
	template <class T>
	static void HistogramFinalize(T value, Vector &result, idx_t offset) {
		FlatVector::GetData<string_t>(result)[offset] = StringVector::AddStringOrBlob(result, value);
	}

	static bool CreateExtraState(idx_t count) {
		return false;
	}

	static void PrepareData(Vector &input, idx_t count, bool &, UnifiedVectorFormat &result) {
		input.ToUnifiedFormat(count, result);
	}
};

struct HistogramGenericFunctor : HistogramStringFunctorBase {
	template <class T>
	static void HistogramFinalize(T value, Vector &result, idx_t offset) {
		CreateSortKeyHelpers::DecodeSortKey(value, result, offset,
		                                    OrderModifiers(OrderType::ASCENDING, OrderByNullType::NULLS_LAST));
	}

	static Vector CreateExtraState(idx_t count) {
		return Vector(LogicalType::BLOB, count);
	}

	static void PrepareData(Vector &input, idx_t count, Vector &extra_state, UnifiedVectorFormat &result) {
		OrderModifiers modifiers(OrderType::ASCENDING, OrderByNullType::NULLS_LAST);
		CreateSortKeyHelpers::CreateSortKey(input, count, modifiers, extra_state);
		input.Flatten(count);
		extra_state.Flatten(count);
		FlatVector::Validity(extra_state).Initialize(FlatVector::Validity(input));
		extra_state.ToUnifiedFormat(count, result);
	}
};

} // namespace goose
