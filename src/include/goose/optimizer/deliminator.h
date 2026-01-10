// Copyright (C) Kumo inc. and its affiliates.
// Author: Jeff.li lijippy@163.com
// All rights reserved.
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

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
