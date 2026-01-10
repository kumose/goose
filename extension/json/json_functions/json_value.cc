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


#include "json_executors.h"

namespace goose {

static void ValueFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	JSONExecutors::BinaryExecute<string_t>(args, state, result, JSONCommon::JSONValue);
}

static void ValueManyFunction(DataChunk &args, ExpressionState &state, Vector &result) {
	JSONExecutors::ExecuteMany<string_t>(args, state, result, JSONCommon::JSONValue);
}

static void GetValueFunctionsInternal(ScalarFunctionSet &set, const LogicalType &input_type) {
	set.AddFunction(ScalarFunction({input_type, LogicalType::BIGINT}, LogicalType::VARCHAR, ValueFunction,
	                               JSONReadFunctionData::Bind, nullptr, nullptr, JSONFunctionLocalState::Init));
	set.AddFunction(ScalarFunction({input_type, LogicalType::VARCHAR}, LogicalType::VARCHAR, ValueFunction,
	                               JSONReadFunctionData::Bind, nullptr, nullptr, JSONFunctionLocalState::Init));
	set.AddFunction(ScalarFunction({input_type, LogicalType::LIST(LogicalType::VARCHAR)},
	                               LogicalType::LIST(LogicalType::VARCHAR), ValueManyFunction,
	                               JSONReadManyFunctionData::Bind, nullptr, nullptr, JSONFunctionLocalState::Init));
}

ScalarFunctionSet JSONFunctions::GetValueFunction() {
	// The value function is just like the extract function but returns NULL if the JSON is not a scalar value
	ScalarFunctionSet set("json_value");
	GetValueFunctionsInternal(set, LogicalType::VARCHAR);
	GetValueFunctionsInternal(set, LogicalType::JSON());
	return set;
}

} // namespace goose
