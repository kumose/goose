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

#include <goose/optimizer/column_binding_replacer.h>

namespace goose {

//! The EmptyResultPullup Optimizer traverses the logical operator tree and Pulls up empty operators when possible
class EmptyResultPullup : LogicalOperatorVisitor {
public:
	EmptyResultPullup() {
	}

	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);

private:
	unique_ptr<LogicalOperator> PullUpEmptyJoinChildren(unique_ptr<LogicalOperator> op);
};

} // namespace goose
