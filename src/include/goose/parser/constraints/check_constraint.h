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

#include <goose/parser/constraint.h>
#include <goose/parser/parsed_expression.h>

namespace goose {

//! The CheckConstraint contains an expression that must evaluate to TRUE for
//! every row in a table
class CheckConstraint : public Constraint {
public:
	static constexpr const ConstraintType TYPE = ConstraintType::CHECK;

public:
	GOOSE_API explicit CheckConstraint(unique_ptr<ParsedExpression> expression);

	unique_ptr<ParsedExpression> expression;

public:
	GOOSE_API string ToString() const override;

	GOOSE_API unique_ptr<Constraint> Copy() const override;

	GOOSE_API void Serialize(Serializer &serializer) const override;
	GOOSE_API static unique_ptr<Constraint> Deserialize(Deserializer &deserializer);
};

} // namespace goose
