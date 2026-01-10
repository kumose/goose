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
