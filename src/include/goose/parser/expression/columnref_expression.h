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
#include <goose/common/vector.h>

namespace goose {
struct BindingAlias;

//! Represents a reference to a column from either the FROM clause or from an
//! alias
class ColumnRefExpression : public ParsedExpression {
public:
	static constexpr const ExpressionClass TYPE = ExpressionClass::COLUMN_REF;

public:
	//! Specify both the column and table name
	ColumnRefExpression(string column_name, string table_name);
	//! Specify both the column and table alias
	ColumnRefExpression(string column_name, const BindingAlias &alias);
	//! Only specify the column name, the table name will be derived later
	explicit ColumnRefExpression(string column_name);
	//! Specify a set of names
	explicit ColumnRefExpression(vector<string> column_names);

	//! The stack of names in order of which they appear (column_names[0].column_names[1].column_names[2]....)
	vector<string> column_names;

public:
	bool IsQualified() const;
	const string &GetColumnName() const;
	const string &GetTableName() const;
	bool IsScalar() const override {
		return false;
	}

	string GetName() const override;
	string ToString() const override;

	static bool Equal(const ColumnRefExpression &a, const ColumnRefExpression &b);
	hash_t Hash() const override;

	unique_ptr<ParsedExpression> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<ParsedExpression> Deserialize(Deserializer &deserializer);

private:
	ColumnRefExpression();
};
} // namespace goose
