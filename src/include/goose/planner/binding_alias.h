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
