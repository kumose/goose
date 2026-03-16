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

#include <goose/parser/parsed_expression.h>
#include <goose/common/optional_ptr.h>

namespace goose {

struct DummyBinding;

//! Represents a reference to a lambda parameter
class LambdaRefExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::LAMBDA_REF;

public:
	//! Constructs a LambdaRefExpression from a lambda_idx and a column_name. We do not specify a table name,
	//! because we use dummy tables to bind lambda parameters
	LambdaRefExpression(idx_t lambda_idx, string column_name_p);

	//! The index of the lambda parameter in the lambda_bindings vector
	idx_t lambda_idx;
	//! The name of the lambda parameter (in a specific Binding in lambda_bindings)
	string column_name;

public:
	bool IsScalar() const override;
	string GetName() const override;
	string ToString() const override;
	hash_t Hash() const override;
	unique_ptr<ParsedExpression> Copy() const override;

	//! Traverses the lambda_bindings to find a matching binding for the column_name
	static unique_ptr<ParsedExpression> FindMatchingBinding(optional_ptr<vector<DummyBinding>> &lambda_bindings,
	                                                        const string &parameter_name);

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);
};
} // namespace goose
