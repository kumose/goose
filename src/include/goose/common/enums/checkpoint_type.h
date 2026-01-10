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

#include <goose/common/constants.h>

namespace goose {

enum class CheckpointWALAction {
	//! Delete the WAL file after the checkpoint completes - generally done on shutdown
	DELETE_WAL,
	//! Leave the WAL file alone
	DONT_DELETE_WAL
};

enum class CheckpointAction {
	//! Checkpoint only if a checkpoint is required (i.e. the WAL has data in it that can be flushed)
	CHECKPOINT_IF_REQUIRED,
	//! Always checkpoint regardless of whether or not there is data in the WAL to flush
	ALWAYS_CHECKPOINT
};

enum class CheckpointType {
	//! Full checkpoints involve vacuuming deleted rows and updates
	//! They can only be run if no transaction need to read old data (that would be cleaned up/vacuumed)
	FULL_CHECKPOINT,
	//! Concurrent checkpoints write committed data to disk but do less clean-up
	//! They can be run even when active transactions need to read old data
	CONCURRENT_CHECKPOINT,
	//! Only run vacuum - this can be triggered for in-memory tables
	VACUUM_ONLY
};

} // namespace goose
