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
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>
#include <goose/common/shared_ptr.h>

namespace goose {
struct StorageLockInternals;

enum class StorageLockType { SHARED = 0, EXCLUSIVE = 1 };

class StorageLockKey {
public:
	StorageLockKey(shared_ptr<StorageLockInternals> internals, StorageLockType type);
	~StorageLockKey();

	StorageLockType GetType() const {
		return type;
	}

private:
	shared_ptr<StorageLockInternals> internals;
	StorageLockType type;
};

class StorageLock {
public:
	StorageLock();
	~StorageLock();

	//! Get an exclusive lock
	unique_ptr<StorageLockKey> GetExclusiveLock();
	//! Get a shared lock
	unique_ptr<StorageLockKey> GetSharedLock();
	//! Try to get an exclusive lock - if we cannot get it immediately we return `nullptr`
	unique_ptr<StorageLockKey> TryGetExclusiveLock();
	//! This is a special method that only exists for checkpointing
	//! This method takes a shared lock, and returns an exclusive lock if the parameter is the only active shared lock
	//! If this method succeeds, we have **both** a shared and exclusive lock active (which normally is not allowed)
	//! But this behavior is required for checkpointing
	unique_ptr<StorageLockKey> TryUpgradeCheckpointLock(StorageLockKey &lock);

private:
	shared_ptr<StorageLockInternals> internals;
};

} // namespace goose
