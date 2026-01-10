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

#include <goose/common/case_insensitive_map.h>
#include <goose/common/hive_partitioning.h>
#include <goose/common/types.h>
#include <goose/main/client_context.h>

namespace goose {
struct BindInfo;
class MultiFileList;

enum class MultiFileColumnMappingMode : uint8_t { BY_NAME, BY_FIELD_ID };

struct MultiFileOptions {
	bool filename = false;
	bool hive_partitioning = false;
	bool auto_detect_hive_partitioning = true;
	bool union_by_name = false;
	bool hive_types_autocast = true;
	MultiFileColumnMappingMode mapping = MultiFileColumnMappingMode::BY_NAME;

	case_insensitive_map_t<LogicalType> hive_types_schema;

	// Default/configurable name of the column containing the file names
	static constexpr const char *DEFAULT_FILENAME_COLUMN = "filename";
	string filename_column = DEFAULT_FILENAME_COLUMN;
	// These are used to pass options through custom multifilereaders
	case_insensitive_map_t<Value> custom_options;

	GOOSE_API void Serialize(Serializer &serializer) const;
	GOOSE_API static MultiFileOptions Deserialize(Deserializer &source);
	GOOSE_API void AddBatchInfo(BindInfo &bind_info) const;
	GOOSE_API void AutoDetectHivePartitioning(MultiFileList &files, ClientContext &context);
	GOOSE_API static bool AutoDetectHivePartitioningInternal(MultiFileList &files, ClientContext &context);
	GOOSE_API void AutoDetectHiveTypesInternal(MultiFileList &files, ClientContext &context);
	GOOSE_API void VerifyHiveTypesArePartitions(const std::map<string, string> &partitions) const;
	GOOSE_API LogicalType GetHiveLogicalType(const string &hive_partition_column) const;
	GOOSE_API Value GetHivePartitionValue(const string &base, const string &entry, ClientContext &context) const;
	GOOSE_API bool AnySet() const;
};

} // namespace goose
