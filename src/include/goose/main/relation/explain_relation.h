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

#include <goose/main/relation.h>

namespace goose {

class ExplainRelation : public Relation {
public:
	explicit ExplainRelation(shared_ptr<Relation> child, ExplainType type = ExplainType::EXPLAIN_STANDARD,
	                         ExplainFormat format = ExplainFormat::DEFAULT);

	shared_ptr<Relation> child;
	vector<ColumnDefinition> columns;
	ExplainType type;
	ExplainFormat format;

public:
	BoundStatement Bind(Binder &binder) override;
	unique_ptr<QueryNode> GetQueryNode() override;
	string GetQuery() override;
	const vector<ColumnDefinition> &Columns() override;
	string ToString(idx_t depth) override;
	bool IsReadOnly() override {
		return false;
	}
};

} // namespace goose
