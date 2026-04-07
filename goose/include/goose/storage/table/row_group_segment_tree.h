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
