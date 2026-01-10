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
