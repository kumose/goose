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

#include <goose/common/common.h>
#include <goose/planner/logical_operator.h>

namespace goose {

class Optimizer;
class ClientContext;

//! The OptimizerExtensionInfo holds static information relevant to the optimizer extension
struct OptimizerExtensionInfo {
	virtual ~OptimizerExtensionInfo() {
	}
};

struct OptimizerExtensionInput {
	ClientContext &context;
	Optimizer &optimizer;
	optional_ptr<OptimizerExtensionInfo> info;
};

typedef void (*optimize_function_t)(OptimizerExtensionInput &input, unique_ptr<LogicalOperator> &plan);
typedef void (*pre_optimize_function_t)(OptimizerExtensionInput &input, unique_ptr<LogicalOperator> &plan);

class OptimizerExtension {
public:
	//! The optimize function of the optimizer extension.
	//! Takes a logical query plan as an input, which it can modify in place
	//! This runs, after the Goose optimizers have run
	optimize_function_t optimize_function = nullptr;
	//! The pre-optimize function of the optimizer extension.
	//! Takes a logical query plan as an input, which it can modify in place
	//! This runs, before the Goose optimizers have run
	pre_optimize_function_t pre_optimize_function = nullptr;

	//! Additional optimizer info passed to the optimize functions
	shared_ptr<OptimizerExtensionInfo> optimizer_info;
};

} // namespace goose
