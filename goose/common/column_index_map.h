#pragma once

#include <goose/common/column_index.h>
#include <goose/common/types-import.h>
#include <goose/common/queue.h>

namespace goose {

struct ColumnIndexHashFunction {
	uint64_t operator()(const ColumnIndex &index) const {
		auto hasher = std::hash<idx_t>();
		queue<reference<const ColumnIndex>> to_hash;

		hash_t result = 0;
		to_hash.push(std::ref(index));
		while (!to_hash.empty()) {
			auto &current = to_hash.front();
			auto &children = current.get().GetChildIndexes();
			for (auto &child : children) {
				to_hash.push(child);
			}
			result ^= hasher(current.get().GetPrimaryIndex());
			to_hash.pop();
		}
		return result;
	}
};

struct ColumnIndexEquality {
	bool operator()(const ColumnIndex &a, const ColumnIndex &b) const {
		return a == b;
	}
};

template <class T>
using column_index_map = unordered_map<ColumnIndex, T, ColumnIndexHashFunction, ColumnIndexEquality>;

using column_index_set = unordered_set<ColumnIndex, ColumnIndexHashFunction, ColumnIndexEquality>;

} // namespace goose
