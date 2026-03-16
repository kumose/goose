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

#include <goose/common/shared_ptr.h>
#include <goose/parser/column_definition.h>

namespace goose {

class ClientContext;

class Relation;

class ClientContextWrapper {
public:
	virtual ~ClientContextWrapper() = default;
	explicit ClientContextWrapper(const shared_ptr<ClientContext> &context);
	shared_ptr<ClientContext> GetContext();
	shared_ptr<ClientContext> TryGetContext();
	virtual void TryBindRelation(Relation &relation, vector<ColumnDefinition> &columns);

private:
	weak_ptr<ClientContext> client_context;
};

} // namespace goose
