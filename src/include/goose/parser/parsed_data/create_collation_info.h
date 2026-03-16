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

#include <goose/parser/parsed_data/create_info.h>
#include <goose/function/scalar_function.h>

namespace goose {

struct CreateCollationInfo : public CreateInfo {
	GOOSE_API CreateCollationInfo(string name_p, ScalarFunction function_p, bool combinable_p,
	                               bool not_required_for_equality_p);

	//! The name of the collation
	string name;
	//! The collation function to push in case collation is required
	ScalarFunction function;
	//! Whether or not the collation can be combined with other collations.
	bool combinable;
	//! Whether or not the collation is required for equality comparisons or not. For many collations a binary
	//! comparison for equality comparisons is correct, allowing us to skip the collation in these cases which greatly
	//! speeds up processing.
	bool not_required_for_equality;

public:
	unique_ptr<CreateInfo> Copy() const override;
};

} // namespace goose
