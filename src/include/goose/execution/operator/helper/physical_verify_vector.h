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

#include <goose/execution/physical_operator.h>
#include <goose/planner/expression.h>
#include <goose/common/enums/debug_vector_verification.h>

namespace goose {

//! The PhysicalVerifyVector operator is a streaming operator that emits the same data as it ingests - but in a
//! different format
// There are different configurations
// * Dictionary: Transform every vector into a dictionary vector where the underlying vector has gaps and is reversed
//       i.e. original: FLAT [1, 2, 3]
//            modified: BASE: [NULL, 3, NULL, 2, NULL, 1]   OFFSETS: [5, 3, 1]
// * Constant: Decompose every DataChunk into single-row constant vectors
//       i.e. original: FLAT [1, 2, 3]
//            modified: chunk #1 - CONSTANT [1]
//                      chunk #2 - CONSTANT [2]
//                      chunk #3 - CONSTANT [3]
// * Sequence & Constant: Decompose every DataChunk into constant or sequence vectors based on the longest possibility
//            original:  a: [1, 1, 20, 15, 13]   b: [1, 10, 100, 101, 102]
//            modified:  chunk #1 - a: CONSTANT [1, 1]          b: DICTIONARY [1, 10]
//                       chunk #2 - a: DICTIONARY [20, 15, 13]  b: SEQUENCE [100, 101, 102]
// * Nested Shuffle: Reshuffle list vectors so that offsets are not contiguous
//            original: [[1, 2], [3, 4]] - BASE: [1, 2, 3, 4] LISTS: [offset: 0, length: 2][offset: 2, length: 2]
//            modified: [[1, 2], [3, 4]] - BASE: [3, 4, 1, 2] LISTS: [offset: 2, length: 2][offset: 0, length: 2]
class PhysicalVerifyVector : public PhysicalOperator {
public:
	static constexpr const PhysicalOperatorType TYPE = PhysicalOperatorType::VERIFY_VECTOR;

public:
	PhysicalVerifyVector(PhysicalPlan &physical_plan, PhysicalOperator &child, DebugVectorVerification verification);

	DebugVectorVerification verification;

public:
	unique_ptr<OperatorState> GetOperatorState(ExecutionContext &context) const override;
	OperatorResultType Execute(ExecutionContext &context, DataChunk &input, DataChunk &chunk,
	                           GlobalOperatorState &gstate, OperatorState &state) const override;

	bool ParallelOperator() const override {
		return true;
	}
};

} // namespace goose
