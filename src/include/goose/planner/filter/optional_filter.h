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
//

#pragma once

#include <goose/planner/table_filter.h>
#include <goose/planner/table_filter_state.h>

namespace goose {

class OptionalFilter : public TableFilter {
public:
	static constexpr auto TYPE = TableFilterType::OPTIONAL_FILTER;

public:
	explicit OptionalFilter(unique_ptr<TableFilter> filter = nullptr);

	//! Optional child filters.
	unique_ptr<TableFilter> child_filter;

public:
	string ToString(const string &column_name) const override;
	unique_ptr<TableFilter> Copy() const override;
	unique_ptr<Expression> ToExpression(const Expression &column) const override;
	FilterPropagateResult CheckStatistics(BaseStatistics &stats) const override;
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableFilter> Deserialize(Deserializer &deserializer);

	virtual void FiltersNullValues(const LogicalType &type, bool &filters_nulls, bool &filters_valid_values,
	                               TableFilterState &filter_state) const {
	}

	virtual unique_ptr<TableFilterState> InitializeState(ClientContext &context) const {
		return make_uniq<TableFilterState>();
	}

	virtual idx_t FilterSelection(SelectionVector &sel, Vector &vector, UnifiedVectorFormat &vdata,
	                              TableFilterState &filter_state, idx_t scan_count, idx_t &approved_tuple_count) const;
};

} // namespace goose
