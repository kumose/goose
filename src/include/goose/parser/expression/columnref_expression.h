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
