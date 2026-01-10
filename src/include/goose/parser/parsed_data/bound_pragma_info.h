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

#include <goose/function/pragma_function.h>

namespace goose {

struct BoundPragmaInfo {
	BoundPragmaInfo(PragmaFunction function_p, vector<Value> parameters_p, named_parameter_map_t named_parameters_p)
	    : function(std::move(function_p)), parameters(std::move(parameters_p)),
	      named_parameters(std::move(named_parameters_p)) {
	}

	PragmaFunction function;
	//! Parameter list (if any)
	vector<Value> parameters;
	//! Named parameter list (if any)
	named_parameter_map_t named_parameters;
};

} // namespace goose
