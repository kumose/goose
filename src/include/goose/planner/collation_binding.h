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

#include <goose/function/cast/default_casts.h>
#include <goose/common/enums/collation_type.h>

namespace goose {
struct MapCastInfo;
struct MapCastNode;
struct DBConfig;

typedef bool (*try_push_collation_t)(ClientContext &context, unique_ptr<Expression> &source,
                                     const LogicalType &sql_type, CollationType type);

struct CollationCallback {
	explicit CollationCallback(try_push_collation_t try_push_collation_p) : try_push_collation(try_push_collation_p) {
	}

	try_push_collation_t try_push_collation;
};

class CollationBinding {
public:
	CollationBinding();

public:
	GOOSE_API static CollationBinding &Get(ClientContext &context);
	GOOSE_API static CollationBinding &Get(DatabaseInstance &db);

	GOOSE_API void RegisterCollation(CollationCallback callback);
	GOOSE_API bool PushCollation(ClientContext &context, unique_ptr<Expression> &source, const LogicalType &sql_type,
	                              CollationType type) const;

private:
	vector<CollationCallback> collations;
};

} // namespace goose
