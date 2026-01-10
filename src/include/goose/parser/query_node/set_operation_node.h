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

#include <goose/common/enums/set_operation_type.h>
#include <goose/parser/parsed_expression.h>
#include <goose/parser/query_node.h>

namespace goose {

class SetOperationNode : public QueryNode {
public:
	static constexpr const QueryNodeType TYPE = QueryNodeType::SET_OPERATION_NODE;

public:
	SetOperationNode();

	//! The type of set operation
	SetOperationType setop_type = SetOperationType::NONE;
	//! whether the ALL modifier was used or not
	bool setop_all = false;
	//! The children of the set operation
	vector<unique_ptr<QueryNode>> children;

public:
	//! Convert the query node to a string
	string ToString() const override;

	bool Equals(const QueryNode *other) const override;
	//! Create a copy of this SelectNode
	unique_ptr<QueryNode> Copy() const override;

	//! Serializes a QueryNode to a stand-alone binary blob
	//! Deserializes a blob back into a QueryNode

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<QueryNode> Deserialize(Deserializer &source);

public:
	// these methods exist for forwards/backwards compatibility of (de)serialization
	SetOperationNode(SetOperationType setop_type, unique_ptr<QueryNode> left, unique_ptr<QueryNode> right,
	                 vector<unique_ptr<QueryNode>> children, bool setop_all);

	unique_ptr<QueryNode> SerializeChildNode(Serializer &serializer, idx_t index) const;
	bool SerializeChildList(Serializer &serializer) const;
};

} // namespace goose
