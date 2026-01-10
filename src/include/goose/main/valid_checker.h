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

#include <goose/common/constants.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>

namespace goose {
class DatabaseInstance;
class MetaTransaction;

class ValidChecker {
public:
	explicit ValidChecker(DatabaseInstance &db);

	GOOSE_API static ValidChecker &Get(DatabaseInstance &db);
	GOOSE_API static ValidChecker &Get(MetaTransaction &transaction);

	GOOSE_API void Invalidate(string error);
	GOOSE_API bool IsInvalidated();
	GOOSE_API string InvalidatedMessage();

	template <class T>
	static bool IsInvalidated(T &o) {
		return Get(o).IsInvalidated();
	}
	template <class T>
	static void Invalidate(T &o, string error) {
		Get(o).Invalidate(std::move(error));
	}

	template <class T>
	static string InvalidatedMessage(T &o) {
		return Get(o).InvalidatedMessage();
	}

private:
	mutex invalidate_lock;
	//! Set to true when encountering a fatal exception.
	atomic<bool> is_invalidated;
	//! The message invalidating the database instance.
	string invalidated_msg;
	//! The database instance.
	DatabaseInstance &db;
};

} // namespace goose
