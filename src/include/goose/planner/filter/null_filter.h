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

namespace goose {

class IsNullFilter : public TableFilter {
public:
	static constexpr const TableFilterType TYPE = TableFilterType::IS_NULL;

public:
	IsNullFilter();

public:
	FilterPropagateResult CheckStatistics(BaseStatistics &stats) const override;
	string ToString(const string &column_name) const override;
	unique_ptr<TableFilter> Copy() const override;
	unique_ptr<Expression> ToExpression(const Expression &column) const override;
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableFilter> Deserialize(Deserializer &deserializer);
};

class IsNotNullFilter : public TableFilter {
public:
	static constexpr const TableFilterType TYPE = TableFilterType::IS_NOT_NULL;

public:
	IsNotNullFilter();

public:
	FilterPropagateResult CheckStatistics(BaseStatistics &stats) const override;
	string ToString(const string &column_name) const override;
	unique_ptr<TableFilter> Copy() const override;
	unique_ptr<Expression> ToExpression(const Expression &column) const override;
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableFilter> Deserialize(Deserializer &deserializer);
};

} // namespace goose
