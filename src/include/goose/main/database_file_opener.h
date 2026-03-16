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

#include <goose/common/file_opener.h>
#include <goose/common/opener_file_system.h>
#include <goose/main/config.h>
#include <goose/main/database.h>
#include <goose/logging/log_manager.h>

namespace goose {
class DatabaseInstance;

class DatabaseFileOpener : public FileOpener {
public:
	explicit DatabaseFileOpener(DatabaseInstance &db_p) : db(db_p) {
	}

	Logger &GetLogger() const override {
		return Logger::Get(db);
	}

	SettingLookupResult TryGetCurrentSetting(const string &key, Value &result) override {
		return db.TryGetCurrentSetting(key, result);
	}

	optional_ptr<ClientContext> TryGetClientContext() override {
		return nullptr;
	}

	optional_ptr<DatabaseInstance> TryGetDatabase() override {
		return &db;
	}
	shared_ptr<HTTPUtil> &GetHTTPUtil() override {
		return TryGetDatabase()->config.http_util;
	}

private:
	DatabaseInstance &db;
};

class DatabaseFileSystem : public OpenerFileSystem {
public:
	explicit DatabaseFileSystem(DatabaseInstance &db_p) : db(db_p), database_opener(db_p) {
	}

	FileSystem &GetFileSystem() const override {
		auto &config = DBConfig::GetConfig(db);
		return *config.file_system;
	}
	optional_ptr<FileOpener> GetOpener() const override {
		return &database_opener;
	}

private:
	DatabaseInstance &db;
	mutable DatabaseFileOpener database_opener;
};

} // namespace goose
