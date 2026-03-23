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
//

#include <goose/common/case_insensitive_map.h>
#include <functional>

#pragma once

namespace goose {

class DependencyItem {
public:
	virtual ~DependencyItem() {};

public:
	template <class TARGET>
	TARGET &Cast() {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<TARGET &>(*this);
	}
	template <class TARGET>
	const TARGET &Cast() const {
		DynamicCastCheck<TARGET>(this);
		return reinterpret_cast<const TARGET &>(*this);
	}
};

using dependency_scan_t = std::function<void(const string &name, shared_ptr<DependencyItem> item)>;

class ExternalDependency {
public:
	explicit ExternalDependency() {
	}
	~ExternalDependency() {
	}

public:
	void AddDependency(const string &name, shared_ptr<DependencyItem> item) {
		objects[name] = std::move(item);
	}
	shared_ptr<DependencyItem> GetDependency(const string &name) const {
		auto it = objects.find(name);
		if (it == objects.end()) {
			return nullptr;
		}
		return it->second;
	}
	void ScanDependencies(const dependency_scan_t &callback) {
		for (auto &kv : objects) {
			callback(kv.first, kv.second);
		}
	}

private:
	//! The objects encompassed by this dependency
	case_insensitive_map_t<shared_ptr<DependencyItem>> objects;
};

} // namespace goose
