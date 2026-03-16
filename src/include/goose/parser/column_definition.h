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
