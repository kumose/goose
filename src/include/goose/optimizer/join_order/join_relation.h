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

#include <goose/common/common.h>
#include <goose/common/types-import.h>
#include <goose/common/types-import.h>

namespace goose {

//! Set of relations, used in the join graph.
struct JoinRelationSet {
	JoinRelationSet(unsafe_unique_array<idx_t> relations, idx_t count) : relations(std::move(relations)), count(count) {
	}

	string ToString() const;

	unsafe_unique_array<idx_t> relations;
	idx_t count;

	static bool IsSubset(JoinRelationSet &super, JoinRelationSet &sub);
};

//! The JoinRelationTree is a structure holding all the created JoinRelationSet objects and allowing fast lookup on to
//! them
class JoinRelationSetManager {
public:
	//! Contains a node with a JoinRelationSet and child relations
	// FIXME: this structure is inefficient, could use a bitmap for lookup instead (todo: profile)
	struct JoinRelationTreeNode {
		unique_ptr<JoinRelationSet> relation;
		unordered_map<idx_t, unique_ptr<JoinRelationTreeNode>> children;
	};

public:
	//! Create or get a JoinRelationSet from a single node with the given index
	JoinRelationSet &GetJoinRelation(idx_t index);
	//! Create or get a JoinRelationSet from a set of relation bindings
	JoinRelationSet &GetJoinRelation(const unordered_set<idx_t> &bindings);
	//! Create or get a JoinRelationSet from a (sorted, duplicate-free!) list of relations
	JoinRelationSet &GetJoinRelation(unsafe_unique_array<idx_t> relations, idx_t count);
	//! Union two sets of relations together and create a new relation set
	JoinRelationSet &Union(JoinRelationSet &left, JoinRelationSet &right);
	// //! Create the set difference of left \ right (i.e. all elements in left that are not in right)
	// JoinRelationSet *Difference(JoinRelationSet *left, JoinRelationSet *right);
	string ToString() const;
	void Print();

private:
	JoinRelationTreeNode root;
};

} // namespace goose
