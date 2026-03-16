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

#include <goose/common/operator/cast_operators.h>
#include <goose/common/string_util.h>

namespace goose {

struct VectorTryCastData {
	VectorTryCastData(Vector &result_p, CastParameters &parameters) : result(result_p), parameters(parameters) {
	}

	Vector &result;
	CastParameters &parameters;
	bool all_converted = true;
};

struct HandleVectorCastError {
	template <class RESULT_TYPE>
	static RESULT_TYPE Operation(const string &error_message, ValidityMask &mask, idx_t idx,
	                             VectorTryCastData &cast_data) {
		HandleCastError::AssignError(error_message, cast_data.parameters);
		cast_data.all_converted = false;
		mask.SetInvalid(idx);
		return NullValue<RESULT_TYPE>();
	}
};

} // namespace goose
