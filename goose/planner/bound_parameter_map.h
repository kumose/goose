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

#include <goose/common/types.h>
#include <goose/common/types-import.h>
#include <goose/planner/expression/bound_parameter_data.h>
#include <goose/common/case_insensitive_map.h>

namespace goose {

class ParameterExpression;
class BoundParameterExpression;

using bound_parameter_map_t = case_insensitive_map_t<shared_ptr<BoundParameterData>>;

struct BoundParameterMap {
public:
	explicit BoundParameterMap(case_insensitive_map_t<BoundParameterData> &parameter_data);

public:
	LogicalType GetReturnType(const string &identifier);

	bound_parameter_map_t *GetParametersPtr();

	const bound_parameter_map_t &GetParameters();

	const case_insensitive_map_t<BoundParameterData> &GetParameterData();

	unique_ptr<BoundParameterExpression> BindParameterExpression(ParameterExpression &expr);

	//! Flag to indicate that we need to rebind this prepared statement before execution
	bool rebind = false;

private:
	shared_ptr<BoundParameterData> CreateOrGetData(const string &identifier);
	void CreateNewParameter(const string &id, const shared_ptr<BoundParameterData> &param_data);

private:
	bound_parameter_map_t parameters;
	// Pre-provided parameter data if populated
	case_insensitive_map_t<BoundParameterData> &parameter_data;
};

} // namespace goose
