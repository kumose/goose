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

#include <goose/common/common.h>
#include <goose/common/types-import.h>
#include <goose/main/client_context.h>
#include <goose/common/vector.h>

namespace goose {
class ClientContext;
class DatabaseInstance;
class Connection;

class ConnectionManager {
public:
	ConnectionManager();

	void AddConnection(ClientContext &context);
	void RemoveConnection(ClientContext &context);

	vector<shared_ptr<ClientContext>> GetConnectionList();
	const reference_map_t<ClientContext, weak_ptr<ClientContext>> &GetConnectionListReference() const {
		return connections;
	}
	idx_t GetConnectionCount() const;

	void AssignConnectionId(Connection &connection);

	static ConnectionManager &Get(DatabaseInstance &db);
	static ConnectionManager &Get(ClientContext &context);

private:
	mutex connections_lock;
	reference_map_t<ClientContext, weak_ptr<ClientContext>> connections;
	atomic<idx_t> connection_count;
	atomic<connection_t> current_connection_id;
};

} // namespace goose
