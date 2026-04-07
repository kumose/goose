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

#include <goose/catalog/catalog_entry/function_entry.h>
#include <goose/catalog/catalog_set.h>
#include <goose/function/function.h>
#include <goose/parser/parsed_data/create_scalar_function_info.h>

namespace goose {
    //! A scalar function in the catalog
    class ScalarFunctionCatalogEntry : public FunctionEntry {
    public:
        static constexpr const CatalogType Type = CatalogType::SCALAR_FUNCTION_ENTRY;
        static constexpr const char *Name = "scalar function";

    public:
        ScalarFunctionCatalogEntry(Catalog &catalog, SchemaCatalogEntry &schema, CreateScalarFunctionInfo &info);

        //! The scalar functions
        ScalarFunctionSet functions;

    public:
        unique_ptr<CatalogEntry> AlterEntry(CatalogTransaction transaction, AlterInfo &info) override;
    };
} // namespace goose
