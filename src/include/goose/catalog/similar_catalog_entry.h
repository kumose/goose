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

#include <goose/common/common.h>
#include <goose/common/optional_ptr.h>

namespace goose {
class SchemaCatalogEntry;

//! Return value of SimilarEntryInSchemas
struct SimilarCatalogEntry {
	//! The entry name. Empty if absent
	string name;
	//! The similarity score of the given name (between 0.0 and 1.0, higher is better)
	double score = 0.0;
	//! The schema of the entry.
	optional_ptr<SchemaCatalogEntry> schema;

	bool Found() const {
		return !name.empty();
	}

	GOOSE_API string GetQualifiedName(bool qualify_catalog, bool qualify_schema) const;
};

} // namespace goose
