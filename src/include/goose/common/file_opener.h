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
