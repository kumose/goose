#pragma once

#include <atomic>
#include <chrono>
#include <list>
#include <mutex>
#include <goose/common/string.h>
#include <goose/common/types.h>
#include <unordered_map>
#include <goose/main/client_context.h>
#include <goose/main/client_context_state.h>

#include <stddef.h>
#include <string>

namespace goose {
	struct HTTPMetadataCacheEntry {
		idx_t length;
		timestamp_t last_modified;
		string etag;
	};

	// Simple cache with a max age for an entry to be valid
	class HTTPMetadataCache : public ClientContextState {
	public:
		explicit HTTPMetadataCache(bool flush_on_query_end_p, bool shared_p)
			: flush_on_query_end(flush_on_query_end_p), shared(shared_p) {
		};

		void Insert(const string &path, HTTPMetadataCacheEntry val) {
			if (shared) {
				lock_guard<mutex> parallel_lock(lock);
				map[path] = val;
			} else {
				map[path] = val;
			}
		};

		void Erase(string path) {
			if (shared) {
				lock_guard<mutex> parallel_lock(lock);
				map.erase(path);
			} else {
				map.erase(path);
			}
		};

		bool Find(string path, HTTPMetadataCacheEntry &ret_val) {
			if (shared) {
				lock_guard<mutex> parallel_lock(lock);
				auto lookup = map.find(path);
				if (lookup != map.end()) {
					ret_val = lookup->second;
					return true;
				} else {
					return false;
				}
			} else {
				auto lookup = map.find(path);
				if (lookup != map.end()) {
					ret_val = lookup->second;
					return true;
				} else {
					return false;
				}
			}
		};

		void Clear() {
			if (shared) {
				lock_guard<mutex> parallel_lock(lock);
				map.clear();
			} else {
				map.clear();
			}
		}

		//! Called by the ClientContext when the current query ends
		void QueryEnd(ClientContext &context) override {
			if (flush_on_query_end) {
				Clear();
			}
		}

	protected:
		mutex lock;
		unordered_map<string, HTTPMetadataCacheEntry> map;
		bool flush_on_query_end;
		bool shared;
	};
} // namespace goose
