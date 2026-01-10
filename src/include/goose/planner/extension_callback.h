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

namespace goose {
class ClientContext;
class DatabaseInstance;
class ErrorData;

class ExtensionCallback {
public:
	virtual ~ExtensionCallback() {
	}

	//! Called when a new connection is opened
	virtual void OnConnectionOpened(ClientContext &context) {
	}
	//! Called when a connection is closed
	virtual void OnConnectionClosed(ClientContext &context) {
	}
	//! Called before an extension starts loading
	virtual void OnBeginExtensionLoad(DatabaseInstance &db, const string &name) {
	}
	//! Called after an extension is finished loading
	virtual void OnExtensionLoaded(DatabaseInstance &db, const string &name) {
	}
	//! Called after an extension fails to load loading
	virtual void OnExtensionLoadFail(DatabaseInstance &db, const string &name, const ErrorData &error) {
	}
};

} // namespace goose
