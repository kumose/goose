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
