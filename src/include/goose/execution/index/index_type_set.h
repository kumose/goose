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

#include <goose/execution/index/index_type.h>
#include <goose/common/types-import.h>
#include <goose/common/case_insensitive_map.h>
#include <goose/common/string.h>
#include <goose/common/optional_ptr.h>

namespace goose {

class IndexTypeSet {
	mutex lock;
	case_insensitive_tree_t<IndexType> functions;

public:
	IndexTypeSet();
	GOOSE_API optional_ptr<IndexType> FindByName(const string &name);
	GOOSE_API void RegisterIndexType(const IndexType &index_type);
};

} // namespace goose
