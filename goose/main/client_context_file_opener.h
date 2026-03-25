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
