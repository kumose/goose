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

#include <goose/common/enums/expression_type.h>
#include <goose/common/enums/filter_propagate_result.h>
#include <goose/common/operator/comparison_operators.h>
#include <goose/common/types/value.h>
#include <goose/storage/statistics/numeric_stats_union.h>
#include <goose/common/array_ptr.h>

namespace goose {
class BaseStatistics;
struct SelectionVector;
class Vector;

struct NumericStatsData {
	//! Whether or not the value has a max value
	bool has_min;
	//! Whether or not the segment has a min value
	bool has_max;
	//! The minimum value of the segment
	NumericValueUnion min;
	//! The maximum value of the segment
	NumericValueUnion max;
};

struct NumericStats {
	//! Unknown statistics - i.e. "has_min" is false, "has_max" is false
	GOOSE_API static BaseStatistics CreateUnknown(LogicalType type);
	//! Empty statistics - i.e. "min = MaxValue<type>, max = MinValue<type>"
	GOOSE_API static BaseStatistics CreateEmpty(LogicalType type);

	//! Returns true if the stats has a constant value
	GOOSE_API static bool IsConstant(const BaseStatistics &stats);
	//! Returns true if the stats has both a min and max value defined
	GOOSE_API static bool HasMinMax(const BaseStatistics &stats);
	//! Returns true if the stats has a min value defined
	GOOSE_API static bool HasMin(const BaseStatistics &stats);
	//! Returns true if the stats has a max value defined
	GOOSE_API static bool HasMax(const BaseStatistics &stats);
	//! Returns the min value - throws an exception if there is no min value
	GOOSE_API static Value Min(const BaseStatistics &stats);
	//! Returns the max value - throws an exception if there is no max value
	GOOSE_API static Value Max(const BaseStatistics &stats);
	//! Sets the min value of the statistics
	GOOSE_API static void SetMin(BaseStatistics &stats, const Value &val);
	//! Sets the max value of the statistics
	GOOSE_API static void SetMax(BaseStatistics &stats, const Value &val);

	template <class T>
	static void SetMax(BaseStatistics &stats, T val) {
		auto &nstats = GetDataUnsafe(stats);
		nstats.has_max = true;
		nstats.max.GetReferenceUnsafe<T>() = val;
	}

	template <class T>
	static void SetMin(BaseStatistics &stats, T val) {
		auto &nstats = GetDataUnsafe(stats);
		nstats.has_min = true;
		nstats.min.GetReferenceUnsafe<T>() = val;
	}

	//! Check whether or not a given comparison with a constant could possibly be satisfied by rows given the statistics
	GOOSE_API static FilterPropagateResult CheckZonemap(const BaseStatistics &stats, ExpressionType comparison_type,
	                                                     array_ptr<const Value> constants);

	GOOSE_API static void Merge(BaseStatistics &stats, const BaseStatistics &other_p);

	GOOSE_API static void Serialize(const BaseStatistics &stats, Serializer &serializer);
	GOOSE_API static void Deserialize(Deserializer &deserializer, BaseStatistics &stats);

	GOOSE_API static string ToString(const BaseStatistics &stats);

	template <class T>
	static inline void UpdateValue(T new_value, T &min, T &max) {
		min = LessThan::Operation(new_value, min) ? new_value : min;
		max = GreaterThan::Operation(new_value, max) ? new_value : max;
	}
	template <class T>
	static inline void Update(NumericStatsData &nstats, T new_value) {
		UpdateValue<T>(new_value, nstats.min.GetReferenceUnsafe<T>(), nstats.max.GetReferenceUnsafe<T>());
	}

	static void Verify(const BaseStatistics &stats, Vector &vector, const SelectionVector &sel, idx_t count);

	template <class T>
	static T GetMin(const BaseStatistics &stats) {
		return NumericStats::Min(stats).GetValueUnsafe<T>();
	}
	template <class T>
	static T GetMax(const BaseStatistics &stats) {
		return NumericStats::Max(stats).GetValueUnsafe<T>();
	}
	template <class T>
	static T GetMinUnsafe(const BaseStatistics &stats);
	template <class T>
	static T GetMaxUnsafe(const BaseStatistics &stats);

private:
	static NumericStatsData &GetDataUnsafe(BaseStatistics &stats);
	static const NumericStatsData &GetDataUnsafe(const BaseStatistics &stats);
	static Value MinOrNull(const BaseStatistics &stats);
	static Value MaxOrNull(const BaseStatistics &stats);
	template <class T>
	static void TemplatedVerify(const BaseStatistics &stats, Vector &vector, const SelectionVector &sel, idx_t count);
};

} // namespace goose
