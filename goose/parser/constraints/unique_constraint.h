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

#include <goose/common/vector.h>
#include <goose/parser/column_list.h>
#include <goose/parser/constraint.h>

namespace goose {

class UniqueConstraint : public Constraint {
public:
	static constexpr const ConstraintType TYPE = ConstraintType::UNIQUE;

public:
	GOOSE_API UniqueConstraint(const LogicalIndex index, const bool is_primary_key);
	GOOSE_API UniqueConstraint(const LogicalIndex index, string column_name, const bool is_primary_key);
	GOOSE_API UniqueConstraint(vector<string> columns, const bool is_primary_key);

public:
	GOOSE_API string ToString() const override;
	GOOSE_API unique_ptr<Constraint> Copy() const override;
	GOOSE_API void Serialize(Serializer &serializer) const override;
	GOOSE_API static unique_ptr<Constraint> Deserialize(Deserializer &deserializer);

	//! Returns true, if the constraint is a PRIMARY KEY constraint.
	bool IsPrimaryKey() const;
	//! Returns true, if the constraint is defined on a single column.
	bool HasIndex() const;
	//! Returns the column index on which the constraint is defined.
	LogicalIndex GetIndex() const;
	//! Sets the column index of the constraint.
	void SetIndex(const LogicalIndex new_index);
	//! Returns a constant reference to the column names on which the constraint is defined.
	const vector<string> &GetColumnNames() const;
	//! Returns a mutable reference to the column names on which the constraint is defined.
	vector<string> &GetColumnNamesMutable();
	//! Returns the column indexes on which the constraint is defined.
	vector<LogicalIndex> GetLogicalIndexes(const ColumnList &columns) const;
	//! Get the name of the constraint.
	string GetName(const string &table_name) const;

private:
	UniqueConstraint();

#ifdef GOOSE_API_1_0
private:
#else
public:
#endif

	//! The indexed column of the constraint. Only used for single-column constraints, invalid otherwise.
	LogicalIndex index;
	//! The names of the columns on which this constraint is defined. Only set if the index field is not set.
	vector<string> columns;
	//! Whether this is a PRIMARY KEY constraint, or a UNIQUE constraint.
	bool is_primary_key;
};

} // namespace goose
