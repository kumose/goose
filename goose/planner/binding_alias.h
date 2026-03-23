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

#include <goose/common/case_insensitive_map.h>

namespace goose {
class StandardEntry;

struct BindingAlias {
	BindingAlias();
	explicit BindingAlias(string alias);
	BindingAlias(string schema, string alias);
	BindingAlias(string catalog, string schema, string alias);
	explicit BindingAlias(const StandardEntry &entry);

	bool IsSet() const;
	const string &GetAlias() const;

	const string &GetCatalog() const {
		return catalog;
	}
	const string &GetSchema() const {
		return schema;
	}

	bool Matches(const BindingAlias &other) const;
	bool operator==(const BindingAlias &other) const;
	string ToString() const;

private:
	string catalog;
	string schema;
	string alias;
};

} // namespace goose
