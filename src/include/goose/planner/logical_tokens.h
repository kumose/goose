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

namespace goose {

class LogicalOperator;

class LogicalAggregate;
class LogicalAnyJoin;
class LogicalColumnDataGet;
class LogicalComparisonJoin;
class LogicalCopyDatabase;
class LogicalCopyToFile;
class LogicalCreate;
class LogicalCreateTable;
class LogicalCreateIndex;
class LogicalCreateTable;
class LogicalCreateSecret;
class LogicalCrossProduct;
class LogicalCTERef;
class LogicalDelete;
class LogicalDelimGet;
class LogicalDistinct;
class LogicalDummyScan;
class LogicalEmptyResult;
class LogicalExecute;
class LogicalExplain;
class LogicalExport;
class LogicalExpressionGet;
class LogicalFilter;
class LogicalGet;
class LogicalInsert;
class LogicalJoin;
class LogicalLimit;
class LogicalMergeInto;
class LogicalOrder;
class LogicalPivot;
class LogicalPositionalJoin;
class LogicalPragma;
class LogicalPrepare;
class LogicalProjection;
class LogicalRecursiveCTE;
class LogicalMaterializedCTE;
class LogicalSetOperation;
class LogicalSample;
class LogicalSimple;
class LogicalVacuum;
class LogicalSet;
class LogicalReset;
class LogicalTopN;
class LogicalUnnest;
class LogicalUpdate;
class LogicalWindow;

} // namespace goose
