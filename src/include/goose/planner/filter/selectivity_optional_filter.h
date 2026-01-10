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

#include <goose/planner/filter/optional_filter.h>

namespace goose {

struct SelectivityOptionalFilterState final : public TableFilterState {
	enum class FilterStatus {
		ACTIVE,
		PAUSED_DUE_TO_ZONE_MAP_STATS, // todo: use this to disable the filter for one zone map based on CheckStatistics
		PAUSED_DUE_TO_HIGH_SELECTIVITY
	};

	struct SelectivityStats {
		idx_t tuples_accepted;
		idx_t tuples_processed;
		idx_t vectors_processed;

		idx_t n_vectors_to_check;
		float selectivity_threshold;

		FilterStatus status;

		SelectivityStats(idx_t n_vectors_to_check, float selectivity_threshold);
		void Update(idx_t accepted, idx_t processed);
		bool IsActive() const;
		double GetSelectivity() const;
	};

	unique_ptr<TableFilterState> child_state;
	SelectivityStats stats;

	explicit SelectivityOptionalFilterState(unique_ptr<TableFilterState> child_state, const idx_t n_vectors_to_check,
	                                        const float selectivity_threshold)
	    : child_state(std::move(child_state)), stats(n_vectors_to_check, selectivity_threshold) {
	}
};

class SelectivityOptionalFilter final : public OptionalFilter {
public:
	static constexpr auto MIN_MAX_THRESHOLD = 0.75f;
	static constexpr idx_t MIN_MAX_CHECK_N = 30;

	static constexpr float BF_THRESHOLD = 0.25f;
	static constexpr idx_t BF_CHECK_N = 75;

	float selectivity_threshold;
	idx_t n_vectors_to_check;

	SelectivityOptionalFilter(unique_ptr<TableFilter> filter, float selectivity_threshold, idx_t n_vectors_to_check);

public:
	unique_ptr<TableFilter> Copy() const override;
	FilterPropagateResult CheckStatistics(BaseStatistics &stats) const override;
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableFilter> Deserialize(Deserializer &deserializer);
	void FiltersNullValues(const LogicalType &type, bool &filters_nulls, bool &filters_valid_values,
	                       TableFilterState &filter_state) const override;
	unique_ptr<TableFilterState> InitializeState(ClientContext &context) const override;
	idx_t FilterSelection(SelectionVector &sel, Vector &vector, UnifiedVectorFormat &vdata,
	                      TableFilterState &filter_state, idx_t scan_count, idx_t &approved_tuple_count) const override;
};
} // namespace goose
