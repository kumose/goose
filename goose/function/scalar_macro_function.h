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
