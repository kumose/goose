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
#include <algorithm>

namespace goose {

//! The FunctionMatcher class contains a set of matchers that can be used to pattern match specific functions
class FunctionMatcher {
public:
	virtual ~FunctionMatcher() {
	}

	virtual bool Match(const string &name) = 0;

	static bool Match(unique_ptr<FunctionMatcher> &matcher, const string &name) {
		if (!matcher) {
			return true;
		}
		return matcher->Match(name);
	}
};

//! The SpecificFunctionMatcher class matches a single specified function name
class SpecificFunctionMatcher : public FunctionMatcher {
public:
	explicit SpecificFunctionMatcher(string name_p) : name(std::move(name_p)) {
	}

	bool Match(const string &matched_name) override {
		return matched_name == this->name;
	}

private:
	string name;
};

//! The ManyFunctionMatcher class matches a set of functions
class ManyFunctionMatcher : public FunctionMatcher {
public:
	explicit ManyFunctionMatcher(unordered_set<string> names_p) : names(std::move(names_p)) {
	}

	bool Match(const string &name) override {
		return names.find(name) != names.end();
	}

private:
	unordered_set<string> names;
};

} // namespace goose
