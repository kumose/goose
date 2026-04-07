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
#include <goose/parser/result_modifier.h>
#include <goose/parser/common_table_expression_info.h>
#include <goose/common/insertion_order_preserving_map.h>
#include <goose/common/exception.h>

namespace goose {

class Deserializer;
class Serializer;

enum class QueryNodeType : uint8_t {
	SELECT_NODE = 1,
	SET_OPERATION_NODE = 2,
	BOUND_SUBQUERY_NODE = 3,
	RECURSIVE_CTE_NODE = 4,
	CTE_NODE = 5,
	STATEMENT_NODE = 6
};

struct CommonTableExpressionInfo;

class CommonTableExpressionMap {
public:
	CommonTableExpressionMap();

	InsertionOrderPreservingMap<unique_ptr<CommonTableExpressionInfo>> map;

public:
	string ToString() const;
	CommonTableExpressionMap Copy() const;

	void Serialize(Serializer &serializer) const;
	// static void Deserialize(Deserializer &deserializer, CommonTableExpressionMap &ret);
	static CommonTableExpressionMap Deserialize(Deserializer &deserializer);
};

class QueryNode {
public:
	explicit QueryNode(QueryNodeType type) : type(type) {
	}
	virtual ~QueryNode() {
	}

	//! The type of the query node, either SetOperation or Select
	QueryNodeType type;
	//! The set of result modifiers associated with this query node
	vector<unique_ptr<ResultModifier>> modifiers;
	//! CTEs (used by SelectNode and SetOperationNode)
	CommonTableExpressionMap cte_map;

public:
	//! Convert the query node to a string
	virtual string ToString() const = 0;

	virtual bool Equals(const QueryNode *other) const;

	//! Create a copy of this QueryNode
	virtual unique_ptr<QueryNode> Copy() const = 0;

	string ResultModifiersToString() const;

	//! Adds a distinct modifier to the query node
	void AddDistinct();

	virtual void Serialize(Serializer &serializer) const;
	static unique_ptr<QueryNode> Deserialize(Deserializer &deserializer);

protected:
	//! Copy base QueryNode properties from another expression to this one,
	//! used in Copy method
	void CopyProperties(QueryNode &other) const;

public:
	template <class TARGET>
	TARGET &Cast() {
		if (type != TARGET::TYPE) {
			throw InternalException("Failed to cast query node to type - query node type mismatch");
		}
		return reinterpret_cast<TARGET &>(*this);
	}

	template <class TARGET>
	const TARGET &Cast() const {
		if (type != TARGET::TYPE) {
			throw InternalException("Failed to cast query node to type - query node type mismatch");
		}
		return reinterpret_cast<const TARGET &>(*this);
	}
};

} // namespace goose
