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

#include <goose/optimizer/column_binding_replacer.h>

namespace goose {

struct DelimCandidate;

//! The Deliminator optimizer traverses the logical operator tree and removes any redundant DelimGets/DelimJoins
class Deliminator {
public:
	Deliminator() {
	}
	//! Perform DelimJoin elimination
	unique_ptr<LogicalOperator> Optimize(unique_ptr<LogicalOperator> op);

private:
	//! Finds DelimJoins and their corresponding DelimGets
	void FindCandidates(unique_ptr<LogicalOperator> &op, vector<DelimCandidate> &candidates);
	void FindJoinWithDelimGet(unique_ptr<LogicalOperator> &op, DelimCandidate &candidate, idx_t depth = 0);
	//! Whether the DelimJoin is selective
	bool HasSelection(const LogicalOperator &delim_join);
	//! Remove joins with a DelimGet
	bool RemoveJoinWithDelimGet(LogicalComparisonJoin &delim_join, const idx_t delim_get_count,
	                            unique_ptr<LogicalOperator> &join, bool &all_equality_conditions);
	bool RemoveInequalityJoinWithDelimGet(LogicalComparisonJoin &delim_join, const idx_t delim_get_count,
	                                      unique_ptr<LogicalOperator> &join,
	                                      const vector<ReplacementBinding> &replacement_bindings);
	void TrySwitchSingleToLeft(LogicalComparisonJoin &delim_join);

private:
	optional_ptr<LogicalOperator> root;
};

} // namespace goose
