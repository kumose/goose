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

#pragma once

#include <goose/goose.h>
#include <goose/main/extension/extension_loader.h>

namespace goose {
    struct HNSWModule {
    public:
        static void Register(ExtensionLoader &loader) {
            auto &db = loader.GetDatabaseInstance();

            RegisterIndex(db);
            RegisterIndexScan(loader);
            RegisterIndexPragmas(loader);
            RegisterMacros(loader);

            // Optimizers
            RegisterExprOptimizer(db);
            RegisterScanOptimizer(db);
            RegisterTopKOptimizer(db);
            RegisterJoinOptimizer(db);
        }

    private:
        static void RegisterIndex(DatabaseInstance &ldb);

        static void RegisterIndexScan(ExtensionLoader &loader);

        static void RegisterIndexPragmas(ExtensionLoader &loader);

        static void RegisterMacros(ExtensionLoader &loader);

        static void RegisterTopKOperator(DatabaseInstance &db);

        static void RegisterExprOptimizer(DatabaseInstance &db);

        static void RegisterTopKOptimizer(DatabaseInstance &db);

        static void RegisterScanOptimizer(DatabaseInstance &db);

        static void RegisterJoinOptimizer(DatabaseInstance &db);
    };
} // namespace goose
