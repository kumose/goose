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

#include <goose/storage/table/segment_tree.h>
#include <goose/storage/table/row_group.h>

namespace goose {
struct DataTableInfo;
class PersistentTableData;
class MetadataReader;

class RowGroupSegmentTree : public SegmentTree<RowGroup, true> {
public:
	RowGroupSegmentTree(RowGroupCollection &collection, idx_t base_row_id);
	~RowGroupSegmentTree() override;

	void Initialize(PersistentTableData &data);

	MetaBlockPointer GetRootPointer() const {
		return root_pointer;
	}

protected:
	shared_ptr<RowGroup> LoadSegment() const override;

	RowGroupCollection &collection;
	mutable idx_t current_row_group;
	mutable idx_t max_row_group;
	mutable unique_ptr<MetadataReader> reader;
	MetaBlockPointer root_pointer;
};

} // namespace goose
