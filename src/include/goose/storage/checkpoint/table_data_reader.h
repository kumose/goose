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

#include <goose/storage/checkpoint_manager.h>

namespace goose {
struct BoundCreateTableInfo;

//! The table data reader is responsible for reading the data of a table from the block manager
class TableDataReader {
public:
	TableDataReader(MetadataReader &reader, BoundCreateTableInfo &info, MetaBlockPointer table_pointer);

	void ReadTableData();

private:
	MetadataReader &reader;
	BoundCreateTableInfo &info;
};

} // namespace goose
