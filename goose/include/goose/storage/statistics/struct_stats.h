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
