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
#include <goose/storage/statistics/base_statistics.h>
#include <goose/storage/statistics/node_statistics.h>

namespace goose {

enum class AggregateType : uint8_t { NON_DISTINCT = 1, DISTINCT = 2 };
//! Whether or not the input order influences the result of the aggregate
enum class AggregateOrderDependent : uint8_t { ORDER_DEPENDENT = 1, NOT_ORDER_DEPENDENT = 2 };
//! Whether or not the input distinctness influences the result of the aggregate
enum class AggregateDistinctDependent : uint8_t { DISTINCT_DEPENDENT = 1, NOT_DISTINCT_DEPENDENT = 2 };
//! Whether or not the combiner needs to preserve the source
enum class AggregateCombineType : uint8_t { PRESERVE_INPUT = 1, ALLOW_DESTRUCTIVE = 2 };

class BoundAggregateExpression;

struct AggregateInputData {
	AggregateInputData(optional_ptr<FunctionData> bind_data_p, ArenaAllocator &allocator_p,
	                   AggregateCombineType combine_type_p = AggregateCombineType::PRESERVE_INPUT)
	    : bind_data(bind_data_p), allocator(allocator_p), combine_type(combine_type_p) {
	}
	optional_ptr<FunctionData> bind_data;
	ArenaAllocator &allocator;
	AggregateCombineType combine_type;
};

struct AggregateUnaryInput {
	AggregateUnaryInput(AggregateInputData &input_p, ValidityMask &input_mask_p)
	    : input(input_p), input_mask(input_mask_p), input_idx(0) {
	}

	AggregateInputData &input;
	ValidityMask &input_mask;
	idx_t input_idx;

	inline bool RowIsValid() {
		return input_mask.RowIsValid(input_idx);
	}
};

struct AggregateBinaryInput {
	AggregateBinaryInput(AggregateInputData &input_p, ValidityMask &left_mask_p, ValidityMask &right_mask_p)
	    : input(input_p), left_mask(left_mask_p), right_mask(right_mask_p) {
	}

	AggregateInputData &input;
	ValidityMask &left_mask;
	ValidityMask &right_mask;
	idx_t lidx;
	idx_t ridx;
};

struct AggregateFinalizeData {
	AggregateFinalizeData(Vector &result_p, AggregateInputData &input_p)
	    : result(result_p), input(input_p), result_idx(0) {
	}

	Vector &result;
	AggregateInputData &input;
	idx_t result_idx;

	inline void ReturnNull() {
		switch (result.GetVectorType()) {
		case VectorType::FLAT_VECTOR:
			FlatVector::SetNull(result, result_idx, true);
			break;
		case VectorType::CONSTANT_VECTOR:
			ConstantVector::SetNull(result, true);
			break;
		default:
			throw InternalException("Invalid result vector type for aggregate");
		}
	}

	inline string_t ReturnString(string_t value) {
		return StringVector::AddStringOrBlob(result, value);
	}
};

struct AggregateStatisticsInput {
	AggregateStatisticsInput(optional_ptr<FunctionData> bind_data_p, vector<BaseStatistics> &child_stats_p,
	                         optional_ptr<NodeStatistics> node_stats_p)
	    : bind_data(bind_data_p), child_stats(child_stats_p), node_stats(node_stats_p) {
	}

	optional_ptr<FunctionData> bind_data;
	vector<BaseStatistics> &child_stats;
	optional_ptr<NodeStatistics> node_stats;
};

} // namespace goose
