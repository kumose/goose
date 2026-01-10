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
