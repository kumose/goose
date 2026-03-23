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
#include <goose/common/types.h>

namespace goose {

//! CastExpression represents a type cast from one SQL type to another SQL type
class CastExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::CAST;

public:
	GOOSE_API CastExpression(LogicalType target, unique_ptr<ParsedExpression> child, bool try_cast = false);

	//! The child of the cast expression
	unique_ptr<ParsedExpression> child;
	//! The type to cast to
	LogicalType cast_type;
	//! Whether or not this is a try_cast expression
	bool try_cast;

public:
	string ToString() const override;

	static bool Equal(const CastExpression &a, const CastExpression &b);

	unique_ptr<ParsedExpression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);

public:
	template <class T, class BASE>
	static string ToString(const T &entry) {
		return (entry.try_cast ? "TRY_CAST(" : "CAST(") + entry.child->ToString() + " AS " +
		       entry.cast_type.ToString() + ")";
	}

private:
	CastExpression();
};
} // namespace goose
