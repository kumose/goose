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

#include <goose/common/common.h>
#include <goose/execution/physical_plan_generator.h>
#include <goose/planner/binder.h>

namespace goose {

//! The OperatorExtensionInfo holds static information relevant to the operator extension
struct OperatorExtensionInfo {
	virtual ~OperatorExtensionInfo() {
	}
};

typedef BoundStatement (*bind_function_t)(ClientContext &context, Binder &binder, OperatorExtensionInfo *info,
                                          SQLStatement &statement);

// forward declaration to avoid circular reference
struct LogicalExtensionOperator;

class OperatorExtension {
public:
	bind_function_t Bind; // NOLINT: backwards compatibility

	//! Additional info passed to the CreatePlan & Bind functions
	shared_ptr<OperatorExtensionInfo> operator_info;

	virtual std::string GetName() = 0;
	virtual unique_ptr<LogicalExtensionOperator> Deserialize(Deserializer &deserializer) = 0;

	virtual ~OperatorExtension() {
	}
};

} // namespace goose
