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

#include <goose/common/file_opener.h>

namespace goose {

class ClientContext;

//! ClientContext-specific FileOpener implementation.
//! This object is owned by ClientContext and never outlives it.
class ClientContextFileOpener : public FileOpener {
public:
	explicit ClientContextFileOpener(ClientContext &context_p) : context(context_p) {
	}

	Logger &GetLogger() const override;
	SettingLookupResult TryGetCurrentSetting(const string &key, Value &result, FileOpenerInfo &info) override;
	SettingLookupResult TryGetCurrentSetting(const string &key, Value &result) override;

	optional_ptr<ClientContext> TryGetClientContext() override {
		return &context;
	}
	optional_ptr<DatabaseInstance> TryGetDatabase() override;
	shared_ptr<HTTPUtil> &GetHTTPUtil() override;

private:
	ClientContext &context;
};

} // namespace goose
