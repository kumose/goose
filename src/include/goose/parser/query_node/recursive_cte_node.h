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
#include <goose/parser/query_node.h>

namespace goose {

class RecursiveCTENode : public QueryNode {
public:
	static constexpr const QueryNodeType TYPE = QueryNodeType::RECURSIVE_CTE_NODE;

public:
	RecursiveCTENode() : QueryNode(QueryNodeType::RECURSIVE_CTE_NODE) {
	}

	string ctename;
	bool union_all;
	//! The left side of the set operation
	unique_ptr<QueryNode> left;
	//! The right side of the set operation
	unique_ptr<QueryNode> right;
	//! Aliases of the recursive CTE node
	vector<string> aliases;
	//! targets for key variants
	vector<unique_ptr<ParsedExpression>> key_targets;

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
};

} // namespace goose
