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


#include <goose/extension/json/json_executors.h>

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
