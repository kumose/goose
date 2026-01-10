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
