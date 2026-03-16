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

#include <goose/common/vector.h>
#include <goose/common/enums/cte_materialize.h>

namespace goose {

class SelectStatement;

struct CommonTableExpressionInfo {
	~CommonTableExpressionInfo();

	vector<string> aliases;
	vector<unique_ptr<ParsedExpression>> key_targets;
	unique_ptr<SelectStatement> query;
	CTEMaterialize materialized = CTEMaterialize::CTE_MATERIALIZE_DEFAULT;

public:
	void Serialize(Serializer &serializer) const;
	static unique_ptr<CommonTableExpressionInfo> Deserialize(Deserializer &deserializer);
	unique_ptr<CommonTableExpressionInfo> Copy();

private:
	CTEMaterialize GetMaterializedForSerialization(Serializer &serializer) const;
};

} // namespace goose
