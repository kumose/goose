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

#include <goose/common/types-import.h>
#include <goose/common/vector.h>
#include <goose/parser/parsed_expression.h>

namespace goose {

enum class LambdaSyntax : uint8_t { DEFAULT = 0, ENABLE_SINGLE_ARROW = 1, DISABLE_SINGLE_ARROW = 2 };
enum class LambdaSyntaxType : uint8_t { SINGLE_ARROW_STORAGE = 0, SINGLE_ARROW = 1, LAMBDA_KEYWORD = 2 };

//! Goose 1.3. introduced a new lambda syntax: lambda x, y: x + y.
//! The new syntax resolves any ambiguity with the JSON arrow operator.
//! Currently, we're still using a LambdaExpression for both cases.
class LambdaExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::LAMBDA;

public:
	LambdaExpression(vector<string> named_parameters_p, unique_ptr<ParsedExpression> expr);
	LambdaExpression(unique_ptr<ParsedExpression> lhs, unique_ptr<ParsedExpression> expr);

	//! The syntax type.
	LambdaSyntaxType syntax_type;
	//! The LHS of a lambda expression or the JSON "->"-operator. We need the context
	//! to determine if the LHS is a list of column references (lambda parameters) or an expression (JSON)
	unique_ptr<ParsedExpression> lhs;
	//! The lambda or JSON expression (RHS)
	unique_ptr<ParsedExpression> expr;
	//! Band-aid for conflicts between lambda binding and JSON binding.
	unique_ptr<ParsedExpression> copied_expr;

public:
	//! Returns a vector to the column references in the LHS expression, and fills the error message,
	//! if the LHS is not a valid lambda parameter list
	vector<reference<const ParsedExpression>> ExtractColumnRefExpressions(string &error_message) const;
	//! Returns the error message for an invalid lambda parameter list
	static string InvalidParametersErrorMessage();
	//! Returns true, if the column_name is a lambda parameter name
	static bool IsLambdaParameter(const vector<unordered_set<string>> &lambda_params, const string &column_name);

	string ToString() const override;
	static bool Equal(const LambdaExpression &a, const LambdaExpression &b);
	hash_t Hash() const override;
	unique_ptr<ParsedExpression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);

private:
	LambdaExpression();
};

} // namespace goose
