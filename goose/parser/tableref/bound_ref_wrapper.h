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

#include <goose/parser/tableref.h>
#include <goose/planner/binder.h>

namespace goose {

//! Represents an already bound table ref - used during binding only
class BoundRefWrapper : public TableRef {
public:
	static constexpr const TableReferenceType TYPE = TableReferenceType::BOUND_TABLE_REF;

public:
	BoundRefWrapper(BoundStatement bound_ref_p, shared_ptr<Binder> binder_p);

	//! The bound reference object
	BoundStatement bound_ref;
	//! The binder that was used to bind this table ref
	shared_ptr<Binder> binder;

public:
	string ToString() const override;
	bool Equals(const TableRef &other_p) const override;
	unique_ptr<TableRef> Copy() override;
	void Serialize(Serializer &serializer) const override;
	static unique_ptr<TableRef> Deserialize(Deserializer &source);
};

} // namespace goose
