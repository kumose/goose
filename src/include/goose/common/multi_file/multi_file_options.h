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
