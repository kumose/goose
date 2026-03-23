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
//
//===----------------------------------------------------------------------===//
//                         Goose
//
// goose/function/scalar_macro_function.hpp
//
//
//===----------------------------------------------------------------------===//

#include <goose/function/scalar_macro_function.h>

#include <goose/function/macro_function.h>
#include <goose/parser/expression/constant_expression.h>
#include <goose/parser/parsed_expression_iterator.h>

namespace goose {

ScalarMacroFunction::ScalarMacroFunction(unique_ptr<ParsedExpression> expression)
    : MacroFunction(MacroType::SCALAR_MACRO), expression(std::move(expression)) {
}

ScalarMacroFunction::ScalarMacroFunction(void) : MacroFunction(MacroType::SCALAR_MACRO) {
}

unique_ptr<MacroFunction> ScalarMacroFunction::Copy() const {
	auto result = make_uniq<ScalarMacroFunction>();
	result->expression = expression->Copy();
	CopyProperties(*result);

	return result;
}

void RemoveQualificationRecursive(unique_ptr<ParsedExpression> &root_expr) {
	ParsedExpressionIterator::VisitExpressionMutable<ColumnRefExpression>(
	    *root_expr, [&](ColumnRefExpression &col_ref) {
		    auto &col_names = col_ref.column_names;
		    if (col_names.size() == 2 && col_names[0].find(DummyBinding::DUMMY_NAME) != string::npos) {
			    col_names.erase(col_names.begin());
		    }
	    });
}

string ScalarMacroFunction::ToSQL() const {
	// In case of nested macro's we need to fix it a bit
	auto expression_copy = expression->Copy();
	RemoveQualificationRecursive(expression_copy);
	return MacroFunction::ToSQL() + StringUtil::Format("(%s)", expression_copy->ToString());
}

} // namespace goose
