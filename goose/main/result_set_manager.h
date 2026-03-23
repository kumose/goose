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

#include <goose/common/types-import.h>
#include <goose/common/shared_ptr.h>
#include <goose/common/reference_map.h>
#include <goose/common/optional_ptr.h>

namespace goose {

class DatabaseInstance;
class ClientContext;
class BlockHandle;
class ColumnDataAllocator;

class ManagedResultSet : public enable_shared_from_this<ManagedResultSet> {
public:
	ManagedResultSet();
	ManagedResultSet(const weak_ptr<DatabaseInstance> &db, vector<shared_ptr<BlockHandle>> &handles);

public:
	bool IsValid() const;
	shared_ptr<DatabaseInstance> GetDatabase() const;
	vector<shared_ptr<BlockHandle>> &GetHandles();

private:
	bool valid;
	weak_ptr<DatabaseInstance> db;
	optional_ptr<vector<shared_ptr<BlockHandle>>> handles;
};

class ResultSetManager {
public:
	explicit ResultSetManager(DatabaseInstance &db);

public:
	static ResultSetManager &Get(ClientContext &context);
	static ResultSetManager &Get(DatabaseInstance &db);
	ManagedResultSet Add(ColumnDataAllocator &allocator);
	void Remove(ColumnDataAllocator &allocator);

private:
	mutex lock;
	weak_ptr<DatabaseInstance> db;
	reference_map_t<ColumnDataAllocator, unique_ptr<vector<shared_ptr<BlockHandle>>>> open_results;
};

} // namespace goose
