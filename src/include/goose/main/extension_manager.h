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
#include <goose/main/extension_install_info.h>

namespace goose {
class ErrorData;

class ExtensionInfo {
public:
	ExtensionInfo();

	mutex lock;
	atomic<bool> is_loaded;
	unique_ptr<ExtensionInstallInfo> install_info;
	unique_ptr<ExtensionLoadedInfo> load_info;
};

class ExtensionActiveLoad {
public:
	ExtensionActiveLoad(DatabaseInstance &db, ExtensionInfo &info, string extension_name);

	DatabaseInstance &db;
	unique_lock<mutex> load_lock;
	ExtensionInfo &info;
	string extension_name;

public:
	void FinishLoad(ExtensionInstallInfo &install_info);
	void LoadFail(const ErrorData &error);
};

class ExtensionManager {
public:
	explicit ExtensionManager(DatabaseInstance &db);

	GOOSE_API bool ExtensionIsLoaded(const string &name);
	GOOSE_API vector<string> GetExtensions();
	GOOSE_API optional_ptr<ExtensionInfo> GetExtensionInfo(const string &name);
	GOOSE_API unique_ptr<ExtensionActiveLoad> BeginLoad(const string &extension);

	GOOSE_API static ExtensionManager &Get(DatabaseInstance &db);
	GOOSE_API static ExtensionManager &Get(ClientContext &context);

private:
	DatabaseInstance &db;
	mutex lock;
	unordered_map<string, unique_ptr<ExtensionInfo>> loaded_extensions_info;
};

} // namespace goose
