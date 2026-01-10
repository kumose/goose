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

#include <goose/common/enums/join_type.h>
#include <goose/common/enums/joinref_type.h>
#include <goose/parser/parsed_expression.h>
#include <goose/parser/tableref.h>
#include <goose/common/vector.h>

namespace goose {

//! Represents a JOIN between two expressions
class JoinRef : public TableRef {
public:
	static constexpr const TableReferenceType TYPE = TableReferenceType::JOIN;

public:
	explicit JoinRef(JoinRefType ref_type = JoinRefType::REGULAR)
	    : TableRef(TableReferenceType::JOIN), type(JoinType::INNER), ref_type(ref_type) {
	}

	//! The left hand side of the join
	unique_ptr<TableRef> left;
	//! The right hand side of the join
	unique_ptr<TableRef> right;
	//! The join condition
	unique_ptr<ParsedExpression> condition;
	//! The join type
	JoinType type;
	//! Join condition type
	JoinRefType ref_type;
	//! The set of USING columns (if any)
	vector<string> using_columns;
	//! Duplicate eliminated columns (if any)
	vector<unique_ptr<ParsedExpression>> duplicate_eliminated_columns;
	//! If we have duplicate eliminated columns if the delim is flipped
	bool delim_flipped = false;
	//! Whether or not this is an implicit cross join
	bool is_implicit = false;

public:
	string ToString() const override;
	bool Equals(const TableRef &other_p) const override;

	unique_ptr<TableRef> Copy() override;

	//! Deserializes a blob back into a JoinRef
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableRef> Deserialize(Deserializer &source);
};
} // namespace goose
