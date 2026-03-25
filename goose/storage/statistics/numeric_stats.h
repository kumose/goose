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

#include <goose/common/enums/expression_type.h>
#include <goose/common/enums/filter_propagate_result.h>
#include <goose/common/operator/comparison_operators.h>
#include <goose/common/types/value.h>
#include <goose/common/types/vector.h>
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

        template<class T>
        static void SetMax(BaseStatistics &stats, T val) {
            auto &nstats = GetDataUnsafe(stats);
            nstats.has_max = true;
            nstats.max.GetReferenceUnsafe<T>() = val;
        }

        template<class T>
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

        template<class T>
        static inline void UpdateValue(T new_value, T &min, T &max) {
            min = LessThan::Operation(new_value, min) ? new_value : min;
            max = GreaterThan::Operation(new_value, max) ? new_value : max;
        }

        template<class T>
        static inline void Update(NumericStatsData &nstats, T new_value) {
            UpdateValue<T>(new_value, nstats.min.GetReferenceUnsafe<T>(), nstats.max.GetReferenceUnsafe<T>());
        }

        static void Verify(const BaseStatistics &stats, Vector &vector, const SelectionVector &sel, idx_t count);

        template<class T>
        static T GetMin(const BaseStatistics &stats) {
            return NumericStats::Min(stats).GetValueUnsafe<T>();
        }

        template<class T>
        static T GetMax(const BaseStatistics &stats) {
            return NumericStats::Max(stats).GetValueUnsafe<T>();
        }

        template<class T>
        static T GetMinUnsafe(const BaseStatistics &stats);

        template<class T>
        static T GetMaxUnsafe(const BaseStatistics &stats);

    private:
        static NumericStatsData &GetDataUnsafe(BaseStatistics &stats);

        static const NumericStatsData &GetDataUnsafe(const BaseStatistics &stats);

        static Value MinOrNull(const BaseStatistics &stats);

        static Value MaxOrNull(const BaseStatistics &stats);

        template<class T>
        static void TemplatedVerify(const BaseStatistics &stats, Vector &vector, const SelectionVector &sel,
                                    idx_t count);
    };

    struct GetNumericValueUnion {
        template<class T>
        static T Operation(const NumericValueUnion &v);
    };

    template<>
    inline int8_t GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.tinyint;
    }

    template<>
    inline int16_t GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.smallint;
    }

    template<>
    inline int32_t GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.integer;
    }

    template<>
    inline int64_t GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.bigint;
    }

    template<>
    inline hugeint_t GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.hugeint;
    }

    template<>
    inline uhugeint_t GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.uhugeint;
    }

    template<>
    inline uint8_t GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.utinyint;
    }

    template<>
    inline uint16_t GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.usmallint;
    }

    template<>
    inline uint32_t GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.uinteger;
    }

    template<>
    inline uint64_t GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.ubigint;
    }

    template<>
    inline float GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.float_;
    }

    template<>
    inline double GetNumericValueUnion::Operation(const NumericValueUnion &v) {
        return v.value_.double_;
    }

    template<class T>
    T NumericStats::GetMinUnsafe(const BaseStatistics &stats) {
        return GetNumericValueUnion::Operation<T>(NumericStats::GetDataUnsafe(stats).min);
    }

    template<class T>
    T NumericStats::GetMaxUnsafe(const BaseStatistics &stats) {
        return GetNumericValueUnion::Operation<T>(NumericStats::GetDataUnsafe(stats).max);
    }

} // namespace goose
