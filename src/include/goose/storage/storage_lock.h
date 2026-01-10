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
