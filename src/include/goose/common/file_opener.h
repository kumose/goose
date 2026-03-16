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

#include <goose/common/string.h>
#include <goose/common/winapi.h>
#include <goose/main/setting_info.h>

namespace goose {

struct CatalogTransaction;
class SecretManager;
class ClientContext;
class HTTPUtil;
class Value;
class Logger;

struct FileOpenerInfo {
	string file_path;
};

//! Abstract type that provide client-specific context to FileSystem.
class FileOpener {
public:
	FileOpener() {
	}
	virtual ~FileOpener() {};

	virtual SettingLookupResult TryGetCurrentSetting(const string &key, Value &result, FileOpenerInfo &info);
	virtual SettingLookupResult TryGetCurrentSetting(const string &key, Value &result) = 0;
	virtual optional_ptr<ClientContext> TryGetClientContext() = 0;
	virtual optional_ptr<DatabaseInstance> TryGetDatabase() = 0;
	virtual shared_ptr<HTTPUtil> &GetHTTPUtil() = 0;

	GOOSE_API virtual Logger &GetLogger() const = 0;
	GOOSE_API static unique_ptr<CatalogTransaction> TryGetCatalogTransaction(optional_ptr<FileOpener> opener);
	GOOSE_API static optional_ptr<ClientContext> TryGetClientContext(optional_ptr<FileOpener> opener);
	GOOSE_API static optional_ptr<DatabaseInstance> TryGetDatabase(optional_ptr<FileOpener> opener);
	GOOSE_API static optional_ptr<SecretManager> TryGetSecretManager(optional_ptr<FileOpener> opener);
	GOOSE_API static SettingLookupResult TryGetCurrentSetting(optional_ptr<FileOpener> opener, const string &key,
	                                                           Value &result);
	GOOSE_API static SettingLookupResult TryGetCurrentSetting(optional_ptr<FileOpener> opener, const string &key,
	                                                           Value &result, FileOpenerInfo &info);

	template <class TYPE>
	static SettingLookupResult TryGetCurrentSetting(optional_ptr<FileOpener> opener, const string &key, TYPE &result,
	                                                optional_ptr<FileOpenerInfo> info) {
		Value output;
		SettingLookupResult lookup_result;

		if (info) {
			lookup_result = TryGetCurrentSetting(opener, key, output, *info);
		} else {
			lookup_result = TryGetCurrentSetting(opener, key, output, *info);
		}

		if (lookup_result) {
			result = output.GetValue<TYPE>();
		}
		return lookup_result;
	}
};

} // namespace goose
