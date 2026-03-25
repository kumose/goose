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

#include <goose/catalog/catalog_entry/copy_function_catalog_entry.h>
#include <goose/parser/parsed_data/create_copy_function_info.h>

namespace goose {
    constexpr const char *CopyFunctionCatalogEntry::Name;

    CopyFunctionCatalogEntry::CopyFunctionCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema,
                                                       CreateCopyFunctionInfo &info)
        : StandardEntry(CatalogType::COPY_FUNCTION_ENTRY, schema, catalog, info.name), function(info.function) {
    }
} // namespace goose
