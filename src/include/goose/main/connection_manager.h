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
