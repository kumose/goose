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
