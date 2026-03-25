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
