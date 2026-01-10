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

#include "column_writer.h"
#include <goose/common/string.h>
#include <goose/common/types/data_chunk.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include "parquet_types.h"

namespace goose {

struct ParquetColumnSchema;
class ParquetReader;
class ColumnReader;
class ClientContext;

struct GeometryColumnReader {
	static unique_ptr<ColumnReader> Create(ParquetReader &reader, const ParquetColumnSchema &schema,
	                                       ClientContext &context);
};

enum class GeoParquetColumnEncoding : uint8_t {
	WKB = 1,
	POINT,
	LINESTRING,
	POLYGON,
	MULTIPOINT,
	MULTILINESTRING,
	MULTIPOLYGON,
};

enum class GeoParquetVersion : uint8_t {
	// Write GeoParquet 1.0 metadata
	// GeoParquet 1.0 has the widest support among readers and writers
	V1,

	// Write GeoParquet 2.0
	// The GeoParquet 2.0 options is identical to GeoParquet 1.0 except the underlying storage
	// of spatial columns is Parquet native geometry, where the Parquet writer will include
	// native statistics according to the underlying Parquet options. Compared to 'BOTH', this will
	// actually write the metadata as containing GeoParquet version 2.0.0
	// However, V2 isnt standardized yet, so this option is still a bit experimental
	V2,

	// Write GeoParquet 1.0 metadata, with native Parquet geometry types
	// This is a bit of a hold-over option for compatibility with systems that
	// reject GeoParquet 2.0 metadata, but can read Parquet native geometry types as they simply ignore the extra
	// logical type. Goose v1.4.0 falls into this category.
	BOTH,

	// Do not write GeoParquet metadata
	// This option suppresses GeoParquet metadata; however, spatial types will be written as
	// Parquet native Geometry/Geography.
	NONE,
};

struct GeoParquetColumnMetadata {
	// The encoding of the geometry column
	GeoParquetColumnEncoding geometry_encoding;

	// The statistics of the geometry column
	GeometryStatsData stats;

	// The crs of the geometry column (if any) in PROJJSON format
	string projjson;

	// Used to track the "primary" geometry column (if any)
	idx_t insertion_index = 0;

	GeoParquetColumnMetadata() {
		geometry_encoding = GeoParquetColumnEncoding::WKB;
		stats.SetEmpty();
	}
};

class GeoParquetFileMetadata {
public:
	explicit GeoParquetFileMetadata(GeoParquetVersion geo_parquet_version) : version(geo_parquet_version) {
	}
	void AddGeoParquetStats(const string &column_name, const LogicalType &type, const GeometryStatsData &stats,
	                        GeoParquetVersion version);
	void Write(goose_parquet::FileMetaData &file_meta_data);

	// Try to read GeoParquet metadata. Returns nullptr if not found, invalid or the required spatial extension is not
	// available.
	static unique_ptr<GeoParquetFileMetadata> TryRead(const goose_parquet::FileMetaData &file_meta_data,
	                                                  const ClientContext &context);
	const unordered_map<string, GeoParquetColumnMetadata> &GetColumnMeta() const;
	optional_ptr<const GeoParquetColumnMetadata> GetColumnMeta(const string &column_name) const;

	bool IsGeometryColumn(const string &column_name) const;

	static bool IsGeoParquetConversionEnabled(const ClientContext &context);

private:
	mutex write_lock;
	unordered_map<string, GeoParquetColumnMetadata> geometry_columns;
	GeoParquetVersion version;
};

} // namespace goose
