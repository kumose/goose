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
