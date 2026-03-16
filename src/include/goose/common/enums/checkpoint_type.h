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
