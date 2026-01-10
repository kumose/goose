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
