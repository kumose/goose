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
