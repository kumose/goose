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

#include "parquet_reader.h"
#include <goose/function/function_set.h>

namespace goose {

class ParquetMetaDataFunction : public TableFunction {
public:
	ParquetMetaDataFunction();
};

class ParquetSchemaFunction : public TableFunction {
public:
	ParquetSchemaFunction();
};

class ParquetKeyValueMetadataFunction : public TableFunction {
public:
	ParquetKeyValueMetadataFunction();
};

class ParquetFileMetadataFunction : public TableFunction {
public:
	ParquetFileMetadataFunction();
};

class ParquetBloomProbeFunction : public TableFunction {
public:
	ParquetBloomProbeFunction();
};

class ParquetFullMetadataFunction : public TableFunction {
public:
	ParquetFullMetadataFunction();
};

} // namespace goose
