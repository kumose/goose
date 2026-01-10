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
