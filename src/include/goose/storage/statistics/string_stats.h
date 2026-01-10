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

#include <goose/common/common.h>
#include <goose/common/enums/expression_type.h>
#include <goose/common/enums/filter_propagate_result.h>
#include <goose/common/exception.h>
#include <goose/common/operator/comparison_operators.h>
#include <goose/common/types/hugeint.h>
#include <goose/common/array_ptr.h>

namespace goose {
class BaseStatistics;
struct SelectionVector;
class Vector;

struct StringStatsData {
	constexpr static uint32_t MAX_STRING_MINMAX_SIZE = 8;

	//! The minimum value of the segment, potentially truncated
	data_t min[MAX_STRING_MINMAX_SIZE];
	//! The maximum value of the segment, potentially truncated
	data_t max[MAX_STRING_MINMAX_SIZE];
	//! Whether or not the column can contain unicode characters
	bool has_unicode;
	//! Whether or not the maximum string length is known
	bool has_max_string_length;
	//! The maximum string length in bytes
	uint32_t max_string_length;
};

struct StringStats {
	//! Unknown statistics - i.e. "has_unicode" is true, "max_string_length" is unknown, "min" is \0, max is \xFF
	GOOSE_API static BaseStatistics CreateUnknown(LogicalType type);
	//! Empty statistics - i.e. "has_unicode" is false, "max_string_length" is 0, "min" is \xFF, max is \x00
	GOOSE_API static BaseStatistics CreateEmpty(LogicalType type);
	//! Whether or not the statistics have a maximum string length defined
	GOOSE_API static bool HasMaxStringLength(const BaseStatistics &stats);
	//! Returns the maximum string length, or throws an exception if !HasMaxStringLength()
	GOOSE_API static uint32_t MaxStringLength(const BaseStatistics &stats);
	//! Whether or not the strings can contain unicode
	GOOSE_API static bool CanContainUnicode(const BaseStatistics &stats);
	//! Returns the min value (up to a length of StringStatsData::MAX_STRING_MINMAX_SIZE)
	GOOSE_API static string Min(const BaseStatistics &stats);
	//! Returns the max value (up to a length of StringStatsData::MAX_STRING_MINMAX_SIZE)
	GOOSE_API static string Max(const BaseStatistics &stats);

	//! Resets the max string length so HasMaxStringLength() is false
	GOOSE_API static void ResetMaxStringLength(BaseStatistics &stats);
	//! Sets the max string length
	GOOSE_API static void SetMaxStringLength(BaseStatistics &stats, uint32_t length);
	//! FIXME: make this part of Set on statistics
	GOOSE_API static void SetContainsUnicode(BaseStatistics &stats);

	GOOSE_API static void Serialize(const BaseStatistics &stats, Serializer &serializer);
	GOOSE_API static void Deserialize(Deserializer &deserializer, BaseStatistics &base);

	GOOSE_API static string ToString(const BaseStatistics &stats);

	GOOSE_API static FilterPropagateResult CheckZonemap(const BaseStatistics &stats, ExpressionType comparison_type,
	                                                     array_ptr<const Value> constants);
	GOOSE_API static FilterPropagateResult CheckZonemap(const_data_ptr_t min_data, idx_t min_len,
	                                                     const_data_ptr_t max_data, idx_t max_len,
	                                                     ExpressionType comparison_type, const string &value);

	GOOSE_API static void Update(BaseStatistics &stats, const string_t &value);
	GOOSE_API static void SetMin(BaseStatistics &stats, const string_t &value);
	GOOSE_API static void SetMax(BaseStatistics &stats, const string_t &value);
	GOOSE_API static void Merge(BaseStatistics &stats, const BaseStatistics &other);
	GOOSE_API static void Verify(const BaseStatistics &stats, Vector &vector, const SelectionVector &sel, idx_t count);

private:
	static StringStatsData &GetDataUnsafe(BaseStatistics &stats);
	static const StringStatsData &GetDataUnsafe(const BaseStatistics &stats);
};

} // namespace goose
