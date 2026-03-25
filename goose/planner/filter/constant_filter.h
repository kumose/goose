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
