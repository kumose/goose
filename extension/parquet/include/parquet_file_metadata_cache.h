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

#include <goose/goose.h>
#include <goose/storage/object_cache.h>
#include "parquet_geometry.h"
#include "parquet_types.h"

namespace goose {
struct CachingFileHandle;
using goose_parquet::FileCryptoMetaData;

enum class ParquetCacheValidity { VALID, INVALID, UNKNOWN };

class ParquetFileMetadataCache : public ObjectCacheEntry {
public:
	ParquetFileMetadataCache(unique_ptr<goose_parquet::FileMetaData> file_metadata, CachingFileHandle &handle,
	                         unique_ptr<GeoParquetFileMetadata> geo_metadata,
	                         unique_ptr<FileCryptoMetaData> crypto_metadata, idx_t footer_size);
	~ParquetFileMetadataCache() override = default;

	//! Parquet file metadata
	unique_ptr<const goose_parquet::FileMetaData> metadata;

	//! GeoParquet metadata
	unique_ptr<GeoParquetFileMetadata> geo_metadata;

	//! Crypto metadata
	unique_ptr<FileCryptoMetaData> crypto_metadata;

	//! Parquet footer size
	idx_t footer_size;

public:
	static string ObjectType();
	string GetObjectType() override;
	optional_idx GetEstimatedCacheMemory() const override;

	bool IsValid(CachingFileHandle &new_handle) const;
	//! Return if a cache entry is valid.
	ParquetCacheValidity IsValid(const OpenFileInfo &info, ClientContext &context) const;

private:
	bool validate;
	timestamp_t last_modified;
	string version_tag;
};

} // namespace goose
