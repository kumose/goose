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

#include <goose/catalog/standard_entry.h>
#include <goose/common/types-import.h>
#include <goose/function/copy_function.h>

namespace goose {
    class Catalog;
    struct CreateCopyFunctionInfo;

    //! A table function in the catalog
    class CopyFunctionCatalogEntry : public StandardEntry {
    public:
        static constexpr const CatalogType Type = CatalogType::COPY_FUNCTION_ENTRY;
        static constexpr const char *Name = "copy function";

    public:
        CopyFunctionCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateCopyFunctionInfo &info);

        //! The copy function
        CopyFunction function;
    };
} // namespace goose
