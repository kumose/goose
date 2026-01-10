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

#include <goose/planner/logical_operator.h>
#include <goose/planner/bound_constraint.h>

namespace goose {
class TableCatalogEntry;

class LogicalDelete : public LogicalOperator {
public:
	static constexpr const LogicalOperatorType TYPE = LogicalOperatorType::LOGICAL_DELETE;

public:
	explicit LogicalDelete(TableCatalogEntry &table, idx_t table_index);

	TableCatalogEntry &table;
	idx_t table_index;
	bool return_chunk;
	vector<unique_ptr<BoundConstraint>> bound_constraints;

public:
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<LogicalOperator> Deserialize(Deserializer &deserializer);

	idx_t EstimateCardinality(ClientContext &context) override;
	vector<idx_t> GetTableIndex() const override;
	string GetName() const override;

protected:
	vector<ColumnBinding> GetColumnBindings() override;
	void ResolveTypes() override;

private:
	LogicalDelete(ClientContext &context, const unique_ptr<CreateInfo> &table_info);
};
} // namespace goose
