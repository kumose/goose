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
#include <goose/common/lru_cache.h>
#include <goose/common/types-import.h>
#include <goose/common/string.h>
#include <goose/common/types-import.h>
#include <goose/main/client_context.h>
#include <goose/main/database.h>

namespace goose {

//! ObjectCache is the base class for objects caches in Goose
class ObjectCacheEntry {
public:
	virtual ~ObjectCacheEntry() {
	}

	virtual string GetObjectType() = 0;

	//! Get the rough cache memory usage in bytes for this entry.
	//! Used for eviction decisions. Return invalid index to prevent eviction.
	virtual optional_idx GetEstimatedCacheMemory() const = 0;
};

class ObjectCache {
public:
	//! Default max memory 8GiB for non-evictable cache entries.
	//
	// TODO(hjiang): Hard-code a large enough memory consumption upper bound, which is likely a non-regression change.
	// I will followup with another PR before v1.5.0 release to provide a user option to tune.
	//
	// A few consideration here: should we cap object cache memory consumption with goose max memory or separate.
	static constexpr idx_t DEFAULT_MAX_MEMORY = 8ULL * 1024 * 1024 * 1024;

	ObjectCache() : ObjectCache(DEFAULT_MAX_MEMORY) {
	}

	explicit ObjectCache(idx_t max_memory) : lru_cache(max_memory) {
	}

	shared_ptr<ObjectCacheEntry> GetObject(const string &key) {
		const lock_guard<mutex> lock(lock_mutex);
		auto non_evictable_it = non_evictable_entries.find(key);
		if (non_evictable_it != non_evictable_entries.end()) {
			return non_evictable_it->second;
		}
		return lru_cache.Get(key);
	}

	template <class T>
	shared_ptr<T> Get(const string &key) {
		shared_ptr<ObjectCacheEntry> object = GetObject(key);
		if (!object || object->GetObjectType() != T::ObjectType()) {
			return nullptr;
		}
		return shared_ptr_cast<ObjectCacheEntry, T>(object);
	}

	template <class T, class... ARGS>
	shared_ptr<T> GetOrCreate(const string &key, ARGS &&... args) {
		const lock_guard<mutex> lock(lock_mutex);

		// Check non-evictable entries first
		auto non_evictable_it = non_evictable_entries.find(key);
		if (non_evictable_it != non_evictable_entries.end()) {
			auto &existing = non_evictable_it->second;
			if (existing->GetObjectType() != T::ObjectType()) {
				return nullptr;
			}
			return shared_ptr_cast<ObjectCacheEntry, T>(existing);
		}

		// Check evictable cache
		auto existing = lru_cache.Get(key);
		if (existing) {
			if (existing->GetObjectType() != T::ObjectType()) {
				return nullptr;
			}
			return shared_ptr_cast<ObjectCacheEntry, T>(existing);
		}

		// Create new entry while holding lock
		auto value = make_shared_ptr<T>(args...);
		const auto estimated_memory = value->GetEstimatedCacheMemory();
		const bool is_evictable = estimated_memory.IsValid();
		if (!is_evictable) {
			non_evictable_entries[key] = value;
		} else {
			lru_cache.Put(key, value, estimated_memory.GetIndex());
		}

		return value;
	}

	void Put(string key, shared_ptr<ObjectCacheEntry> value) {
		if (!value) {
			return;
		}

		const lock_guard<mutex> lock(lock_mutex);
		const auto estimated_memory = value->GetEstimatedCacheMemory();
		const bool is_evictable = estimated_memory.IsValid();
		if (!is_evictable) {
			non_evictable_entries[std::move(key)] = std::move(value);
			return;
		}
		lru_cache.Put(std::move(key), std::move(value), estimated_memory.GetIndex());
	}

	void Delete(const string &key) {
		const lock_guard<mutex> lock(lock_mutex);
		auto iter = non_evictable_entries.find(key);
		if (iter != non_evictable_entries.end()) {
			non_evictable_entries.erase(iter);
			return;
		}
		lru_cache.Delete(key);
	}

	GOOSE_API static ObjectCache &GetObjectCache(ClientContext &context);

	idx_t GetMaxMemory() const {
		const lock_guard<mutex> lock(lock_mutex);
		return lru_cache.MaxMemory();
	}
	idx_t GetCurrentMemory() const {
		const lock_guard<mutex> lock(lock_mutex);
		return lru_cache.CurrentMemory();
	}
	size_t GetEntryCount() const {
		const lock_guard<mutex> lock(lock_mutex);
		return lru_cache.Size() + non_evictable_entries.size();
	}

private:
	mutable mutex lock_mutex;
	//! LRU cache for evictable entries
	SharedLruCache<string, ObjectCacheEntry> lru_cache;
	//! Separate storage for non-evictable entries (i.e., encryption keys)
	unordered_map<string, shared_ptr<ObjectCacheEntry>> non_evictable_entries;
};

} // namespace goose
