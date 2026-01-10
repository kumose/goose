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
#include "parquet_types.h"

namespace goose {

using namespace goose_parquet; // NOLINT

using goose_parquet::ConvertedType;
using goose_parquet::FieldRepetitionType;
using goose_parquet::SchemaElement;

using goose_parquet::FileMetaData;
struct ParquetOptions;

enum class ParquetColumnSchemaType { COLUMN, FILE_ROW_NUMBER, EXPRESSION, VARIANT, GEOMETRY };

enum class ParquetExtraTypeInfo {
	NONE,
	IMPALA_TIMESTAMP,
	UNIT_NS,
	UNIT_MS,
	UNIT_MICROS,
	DECIMAL_BYTE_ARRAY,
	DECIMAL_INT32,
	DECIMAL_INT64,
	FLOAT16
};

struct ParquetColumnSchema {
public:
	ParquetColumnSchema() = default;
	ParquetColumnSchema(ParquetColumnSchema &&other) = default;
	ParquetColumnSchema(const ParquetColumnSchema &other) = default;
	ParquetColumnSchema &operator=(ParquetColumnSchema &&other) = default;

public:
	//! Writer constructors
	static ParquetColumnSchema FromLogicalType(const string &name, const LogicalType &type, idx_t max_define,
	                                           idx_t max_repeat, idx_t column_index,
	                                           goose_parquet::FieldRepetitionType::type repetition_type,
	                                           bool allow_geometry,
	                                           ParquetColumnSchemaType schema_type = ParquetColumnSchemaType::COLUMN);

public:
	//! Reader constructors
	static ParquetColumnSchema FromSchemaElement(const SchemaElement &element, idx_t max_define, idx_t max_repeat,
	                                             idx_t schema_index, idx_t column_index, ParquetColumnSchemaType type,
	                                             const ParquetOptions &options);
	static ParquetColumnSchema FromParentSchema(ParquetColumnSchema parent, LogicalType result_type,
	                                            ParquetColumnSchemaType schema_type);
	static ParquetColumnSchema FromChildSchemas(const string &name, const LogicalType &type, idx_t max_define,
	                                            idx_t max_repeat, idx_t schema_index, idx_t column_index,
	                                            vector<ParquetColumnSchema> &&children,
	                                            ParquetColumnSchemaType schema_type = ParquetColumnSchemaType::COLUMN);
	static ParquetColumnSchema FileRowNumber();

public:
	unique_ptr<BaseStatistics> Stats(const FileMetaData &file_meta_data, const ParquetOptions &parquet_options,
	                                 idx_t row_group_idx_p, const vector<goose_parquet::ColumnChunk> &columns) const;

public:
	void SetSchemaIndex(idx_t schema_idx);

public:
	string name;
	idx_t max_define;
	idx_t max_repeat;
	//! Populated by FinalizeSchema if used in the parquet_writer path
	optional_idx schema_index;
	idx_t column_index;
	ParquetColumnSchemaType schema_type;
	LogicalType type;
	optional_idx parent_schema_index;
	uint32_t type_length = 0;
	uint32_t type_scale = 0;
	goose_parquet::Type::type parquet_type = goose_parquet::Type::INT32;
	ParquetExtraTypeInfo type_info = ParquetExtraTypeInfo::NONE;
	vector<ParquetColumnSchema> children;
	optional_idx field_id;
	//! Whether a column is nullable or not
	goose_parquet::FieldRepetitionType::type repetition_type = goose_parquet::FieldRepetitionType::OPTIONAL;
	//! Whether the column can be recognized as a GEOMETRY type
	bool allow_geometry = false;
};

} // namespace goose
