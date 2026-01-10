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
#include <goose/parser/tableref.h>
#include <goose/parser/parsed_data/sample_options.h>
#include <goose/parser/group_by_node.h>
#include <goose/common/enums/aggregate_handling.h>

namespace goose {

//! SelectNode represents a standard SELECT statement
class SelectNode : public QueryNode {
public:
	static constexpr const QueryNodeType TYPE = QueryNodeType::SELECT_NODE;

public:
	GOOSE_API SelectNode();

	//! The projection list
	vector<unique_ptr<ParsedExpression>> select_list;
	//! The FROM clause
	unique_ptr<TableRef> from_table;
	//! The WHERE clause
	unique_ptr<ParsedExpression> where_clause;
	//! list of groups
	GroupByNode groups;
	//! HAVING clause
	unique_ptr<ParsedExpression> having;
	//! QUALIFY clause
	unique_ptr<ParsedExpression> qualify;
	//! Aggregate handling during binding
	AggregateHandling aggregate_handling;
	//! The SAMPLE clause
	unique_ptr<SampleOptions> sample;

public:
	//! Convert the query node to a string
	string ToString() const override;

	bool Equals(const QueryNode *other) const override;

	//! Create a copy of this SelectNode
	unique_ptr<QueryNode> Copy() const override;

	//! Serializes a QueryNode to a stand-alone binary blob

	//! Deserializes a blob back into a QueryNode

	void Serialize(Serializer &serializer) const override;
	static unique_ptr<QueryNode> Deserialize(Deserializer &deserializer);
};

} // namespace goose
