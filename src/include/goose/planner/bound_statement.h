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

#include <goose/common/string.h>
#include <goose/common/unique_ptr.h>
#include <goose/common/vector.h>
#include <goose/common/enums/set_operation_type.h>
#include <goose/common/shared_ptr.h>

namespace goose {

class LogicalOperator;
struct LogicalType;
struct BoundStatement;
class ParsedExpression;
class Binder;

struct ExtraBoundInfo {
	SetOperationType setop_type = SetOperationType::NONE;
	vector<shared_ptr<Binder>> child_binders;
	vector<BoundStatement> bound_children;
	vector<unique_ptr<ParsedExpression>> original_expressions;
};

struct BoundStatement {
	unique_ptr<LogicalOperator> plan;
	vector<LogicalType> types;
	vector<string> names;
	ExtraBoundInfo extra_info;
};

} // namespace goose
