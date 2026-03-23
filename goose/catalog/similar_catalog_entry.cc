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
//


#include <goose/catalog/similar_catalog_entry.h>
#include <goose/catalog/catalog_entry/schema_catalog_entry.h>
#include <goose/catalog/catalog.h>

namespace goose {
    string SimilarCatalogEntry::GetQualifiedName(bool qualify_catalog, bool qualify_schema) const {
        D_ASSERT(Found());
        string result;
        if (qualify_catalog) {
            result += schema->catalog.GetName();
        }
        if (qualify_schema) {
            if (!result.empty()) {
                result += ".";
            }
            result += schema->name;
        }
        if (!result.empty()) {
            result += ".";
        }
        result += name;
        return result;
    }
} // namespace goose
