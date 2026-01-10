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
