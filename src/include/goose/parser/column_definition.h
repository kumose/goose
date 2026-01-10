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

#include <goose/common/common.h>
#include <goose/common/types/value.h>
#include <goose/parser/parsed_expression.h>
#include <goose/common/enums/compression_type.h>
#include <goose/catalog/catalog_entry/table_column_type.h>

namespace goose {

struct RenameColumnInfo;
struct RenameTableInfo;

class ColumnDefinition;

//! A column of a table.
class ColumnDefinition {
public:
	GOOSE_API ColumnDefinition(string name, LogicalType type);
	GOOSE_API ColumnDefinition(string name, LogicalType type, unique_ptr<ParsedExpression> expression,
	                            TableColumnType category);

public:
	//! default_value
	const ParsedExpression &DefaultValue() const;
	bool HasDefaultValue() const;
	void SetDefaultValue(unique_ptr<ParsedExpression> default_value);

	//! type
	GOOSE_API const LogicalType &Type() const;
	LogicalType &TypeMutable();
	void SetType(const LogicalType &type);

	//! name
	GOOSE_API const string &Name() const;
	void SetName(const string &name);

	//! comment
	GOOSE_API const Value &Comment() const;
	void SetComment(const Value &comment);

	//! compression_type
	const goose::CompressionType &CompressionType() const;
	void SetCompressionType(goose::CompressionType compression_type);

	//! storage_oid
	const storage_t &StorageOid() const;
	void SetStorageOid(storage_t storage_oid);

	LogicalIndex Logical() const;
	PhysicalIndex Physical() const;

	//! oid
	const column_t &Oid() const;
	void SetOid(column_t oid);

	//! category
	const TableColumnType &Category() const;
	//! Whether this column is a Generated Column
	bool Generated() const;
	GOOSE_API ColumnDefinition Copy() const;

	GOOSE_API void Serialize(Serializer &serializer) const;
	GOOSE_API static ColumnDefinition Deserialize(Deserializer &deserializer);

	//===--------------------------------------------------------------------===//
	// Generated Columns (VIRTUAL)
	//===--------------------------------------------------------------------===//

	ParsedExpression &GeneratedExpressionMutable();
	const ParsedExpression &GeneratedExpression() const;
	void SetGeneratedExpression(unique_ptr<ParsedExpression> expression);
	void ChangeGeneratedExpressionType(const LogicalType &type);
	void GetListOfDependencies(vector<string> &dependencies) const;

	string GetName() const;

	LogicalType GetType() const;

private:
	//! The name of the entry
	string name;
	//! The type of the column
	LogicalType type;
	//! Compression Type used for this column
	goose::CompressionType compression_type = goose::CompressionType::COMPRESSION_AUTO;
	//! The index of the column in the storage of the table
	storage_t storage_oid = DConstants::INVALID_INDEX;
	//! The index of the column in the table
	idx_t oid = DConstants::INVALID_INDEX;
	//! The category of the column
	TableColumnType category = TableColumnType::STANDARD;
	//! The default value of the column (for non-generated columns)
	//! The generated column expression (for generated columns)
	unique_ptr<ParsedExpression> expression;
	//! Comment on this column
	Value comment;
	//! Tags on this column
	unordered_map<string, string> tags;
};

} // namespace goose
