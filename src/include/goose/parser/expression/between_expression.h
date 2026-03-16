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

class BetweenExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::BETWEEN;

public:
	GOOSE_API BetweenExpression(unique_ptr<ParsedExpression> input, unique_ptr<ParsedExpression> lower,
	                             unique_ptr<ParsedExpression> upper);

	unique_ptr<ParsedExpression> input;
	unique_ptr<ParsedExpression> lower;
	unique_ptr<ParsedExpression> upper;

public:
	string ToString() const override;

	static bool Equal(const BetweenExpression &a, const BetweenExpression &b);

	unique_ptr<ParsedExpression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);

public:
	template <class T, class BASE>
	static string ToString(const T &entry) {
		return "(" + entry.input->ToString() + " BETWEEN " + entry.lower->ToString() + " AND " +
		       entry.upper->ToString() + ")";
	}

private:
	BetweenExpression();
};
} // namespace goose
