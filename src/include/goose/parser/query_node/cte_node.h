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

#include <goose/parser/parsed_expression.h>
#include <goose/parser/query_node.h>

namespace goose {

//! DEPRECATED - CTENode is only preserved for backwards compatibility when serializing older databases
class CTENode : public QueryNode {
public:
	static constexpr const QueryNodeType TYPE = QueryNodeType::CTE_NODE;

public:
	CTENode() : QueryNode(QueryNodeType::CTE_NODE) {
	}

	string ctename;
	unique_ptr<QueryNode> query;
	unique_ptr<QueryNode> child;
	vector<string> aliases;

	CTEMaterialize materialized = CTEMaterialize::CTE_MATERIALIZE_DEFAULT;

public:
	string ToString() const override;

	bool Equals(const QueryNode *other) const override;
	unique_ptr<QueryNode> Copy() const override;

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<QueryNode> Deserialize(Deserializer &source);
};

} // namespace goose
