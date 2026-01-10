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
//! The SelectStatement of the view
#include <goose/function/macro_function.h>
#include <goose/parser/query_node.h>
#include <goose/function/function.h>
#include <goose/main/client_context.h>
#include <goose/planner/binder.h>
#include <goose/planner/expression_binder.h>
#include <goose/parser/expression/constant_expression.h>

namespace goose {

class ScalarMacroFunction : public MacroFunction {
public:
	static constexpr const MacroType TYPE = MacroType::SCALAR_MACRO;

public:
	explicit ScalarMacroFunction(unique_ptr<ParsedExpression> expression);
	ScalarMacroFunction(void);

	//! The macro expression
	unique_ptr<ParsedExpression> expression;

public:
	unique_ptr<MacroFunction> Copy() const override;

	string ToSQL() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<MacroFunction> Deserialize(Deserializer &deserializer);
};

} // namespace goose
