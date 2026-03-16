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
