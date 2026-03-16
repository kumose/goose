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

namespace goose {

// Parameters come in three different types:
// auto-increment:
//	token: '?'
//	name: -
//	number: 0
// positional:
//	token: '$<number>'
//	name: -
//	number: <number>
// named:
//	token: '$<name>'
//	name: <name>
//	number: 0
enum class PreparedParamType : uint8_t { AUTO_INCREMENT, POSITIONAL, NAMED, INVALID };

class ParameterExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::PARAMETER;

public:
	ParameterExpression();

	string identifier;

public:
	bool IsScalar() const override {
		return true;
	}
	bool HasParameter() const override {
		return true;
	}

	string ToString() const override;

	static bool Equal(const ParameterExpression &a, const ParameterExpression &b);

	unique_ptr<ParsedExpression> Copy() const override;
	hash_t Hash() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);
};
} // namespace goose
