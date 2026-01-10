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

#include <goose/planner/table_filter.h>
#include <goose/common/types/value.h>
#include <goose/common/enums/expression_type.h>

namespace goose {

class ConstantFilter : public TableFilter {
public:
	static constexpr const TableFilterType TYPE = TableFilterType::CONSTANT_COMPARISON;

public:
	ConstantFilter(ExpressionType comparison_type, Value constant);

	//! The comparison type (e.g. COMPARE_EQUAL, COMPARE_GREATERTHAN, COMPARE_LESSTHAN, ...)
	ExpressionType comparison_type;
	//! The constant value to filter on
	Value constant;

public:
	bool Compare(const Value &value) const;
	FilterPropagateResult CheckStatistics(BaseStatistics &stats) const override;
	string ToString(const string &column_name) const override;
	bool Equals(const TableFilter &other) const override;
	unique_ptr<TableFilter> Copy() const override;
	unique_ptr<Expression> ToExpression(const Expression &column) const override;
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableFilter> Deserialize(Deserializer &deserializer);
};

} // namespace goose
