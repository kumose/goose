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

#include <goose/planner/expression_binder/base_select_binder.h>
#include <goose/planner/expression_binder/column_alias_binder.h>

namespace goose {
struct SelectBindState;

//! The QUALIFY binder is responsible for binding an expression within the QUALIFY clause of a SQL statement
class QualifyBinder : public BaseSelectBinder {
public:
	QualifyBinder(Binder &binder, ClientContext &context, BoundSelectNode &node, BoundGroupInformation &info);

protected:
	BindResult BindColumnRef(unique_ptr<ParsedExpression> &expr_ptr, idx_t depth, bool root_expression) override;

private:
	ColumnAliasBinder column_alias_binder;
};

} // namespace goose
