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

#include <functional>

#include <goose/common/common.h>
#include <goose/common/shared_ptr.h>
#include <goose/common/string.h>
#include <goose/common/types-import.h>

namespace goose {
    // A LRU cache implementation, whose value could be accessed in a shared manner with shared pointer.
    // Notice, it's not thread-safe.
    template<typename Key, typename Val, typename KeyHash = std::hash<Key>, typename KeyEqual = std::equal_to<Key> >
    class SharedLruCache {
    public:
        using key_type = Key;
        using mapped_type = shared_ptr<Val>;
        using hasher = KeyHash;
        using key_equal = KeyEqual;

        // @param max_memory_p: Maximum total memory (in bytes) of entries. 0 means unlimited.
        explicit SharedLruCache(idx_t max_memory_p) : max_memory(max_memory_p), current_memory(0) {
        }

        // Disable copy and move
        SharedLruCache(const SharedLruCache &) = delete;

        SharedLruCache &operator=(const SharedLruCache &) = delete;

        ~SharedLruCache() = default;

        // Insert `value` with key `key` and explicit memory size. This will replace any previous entry with the same key.
        void Put(Key key, shared_ptr<Val> value, idx_t memory_size) {
            // Remove existing entry if present
            auto existing_it = entry_map.find(key);
            if (existing_it != entry_map.end()) {
                DeleteImpl(existing_it);
            }

            // Evict entries if needed to make room
            if (max_memory > 0 && memory_size > 0) {
                EvictIfNeeded(memory_size);
            }

            // Add new entry
            lru_list.emplace_front(key);
            Entry new_entry;
            new_entry.value = std::move(value);
            new_entry.memory = memory_size;
            new_entry.lru_iterator = lru_list.begin();

            entry_map[std::move(key)] = std::move(new_entry);
            current_memory += memory_size;
        }

        // Delete the entry with key `key`.
        // Return whether the requested `key` is found in the cache.
        bool Delete(const Key &key) {
            auto it = entry_map.find(key);
            if (it == entry_map.end()) {
                return false;
            }
            DeleteImpl(it);
            return true;
        }

        // Look up the entry with key `key`. Return nullptr if not found.
        shared_ptr<Val> Get(const Key &key) {
            auto entry_map_iter = entry_map.find(key);
            if (entry_map_iter == entry_map.end()) {
                return nullptr;
            }

            // Move to front, which indicates most recently used.
            lru_list.splice(lru_list.begin(), lru_list, entry_map_iter->second.lru_iterator);
            return entry_map_iter->second.value;
        }

        // Clear the whole cache.
        void Clear() {
            entry_map.clear();
            lru_list.clear();
            current_memory = 0;
        }

        idx_t MaxMemory() const {
            return max_memory;
        }

        idx_t CurrentMemory() const {
            return current_memory;
        }

        size_t Size() const {
            return entry_map.size();
        }

    private:
        struct Entry {
            shared_ptr<Val> value;
            idx_t memory;
            typename list<Key>::iterator lru_iterator;
        };

        using EntryMap = unordered_map<Key, Entry, KeyHash, KeyEqual>;

        void DeleteImpl(typename EntryMap::iterator iter) {
            current_memory -= iter->second.memory;
            D_ASSERT(current_memory >= 0);
            lru_list.erase(iter->second.lru_iterator);
            entry_map.erase(iter);
        }

        void EvictIfNeeded(idx_t required_memory) {
            if (max_memory == 0) {
                return;
            }

            // Evict LRU entries until we have enough space
            while (!lru_list.empty() && (current_memory + required_memory > max_memory)) {
                const auto &stale_key = lru_list.back();
                auto stale_it = entry_map.find(stale_key);
                if (stale_it != entry_map.end()) {
                    DeleteImpl(stale_it);
                } else {
                    // Should not happen, but be defensive
                    lru_list.pop_back();
                }
            }
        }

        const idx_t max_memory;
        idx_t current_memory;
        EntryMap entry_map;
        list<Key> lru_list;
    };
} // namespace goose
