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
