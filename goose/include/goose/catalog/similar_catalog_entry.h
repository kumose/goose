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
