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
#include <goose/common/exception.h>

namespace goose {
class BaseStatistics;
struct SelectionVector;
class Vector;
struct StorageIndex;

struct StructStats {
	GOOSE_API static void Construct(BaseStatistics &stats);
	GOOSE_API static BaseStatistics CreateUnknown(LogicalType type);
	GOOSE_API static BaseStatistics CreateEmpty(LogicalType type);

	GOOSE_API static const BaseStatistics *GetChildStats(const BaseStatistics &stats);
	GOOSE_API static const BaseStatistics &GetChildStats(const BaseStatistics &stats, idx_t i);
	GOOSE_API static BaseStatistics &GetChildStats(BaseStatistics &stats, idx_t i);
	GOOSE_API static void SetChildStats(BaseStatistics &stats, idx_t i, const BaseStatistics &new_stats);
	GOOSE_API static void SetChildStats(BaseStatistics &stats, idx_t i, unique_ptr<BaseStatistics> new_stats);

	GOOSE_API static void Serialize(const BaseStatistics &stats, Serializer &serializer);
	GOOSE_API static void Deserialize(Deserializer &deserializer, BaseStatistics &base);

	GOOSE_API static string ToString(const BaseStatistics &stats);

	GOOSE_API static void Merge(BaseStatistics &stats, const BaseStatistics &other);
	GOOSE_API static void Copy(BaseStatistics &stats, const BaseStatistics &other);
	GOOSE_API static void Verify(const BaseStatistics &stats, Vector &vector, const SelectionVector &sel, idx_t count);

	GOOSE_API static unique_ptr<BaseStatistics> PushdownExtract(const BaseStatistics &stats,
	                                                             const StorageIndex &index);
};

} // namespace goose
