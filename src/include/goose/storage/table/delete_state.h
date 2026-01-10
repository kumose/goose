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

#include <goose/storage/table/append_state.h>

namespace goose {
class TableCatalogEntry;

struct TableDeleteState {
	unique_ptr<ConstraintState> constraint_state;
	bool has_delete_constraints = false;
	DataChunk verify_chunk;
	vector<StorageIndex> col_ids;
	shared_ptr<CheckpointLock> checkpoint_lock;
};

} // namespace goose
