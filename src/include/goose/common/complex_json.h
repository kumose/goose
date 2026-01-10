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

#include <unordered_map>
#include <string>
#include <goose/common/exception.h>
#include <goose/common/vector.h>
#include <goose/common/unique_ptr.h>

namespace goose {

enum class ComplexJSONType : uint8_t { VALUE = 0, OBJECT = 1, ARRAY = 2 };

//! Custom struct to handle both strings and nested JSON objects
struct ComplexJSON {
	//! Constructor for string values
	explicit ComplexJSON(const string &str);
	//! Basic empty constructor
	ComplexJSON();
	//! Adds entry to the underlying map, also sets the type to OBJECT
	void AddObjectEntry(const string &key, unique_ptr<ComplexJSON> object);
	//! Adds element to the underlying list, also sets the type to ARRAY
	void AddArrayElement(unique_ptr<ComplexJSON> object);
	//! Gets a ComplexJSON object from the map
	ComplexJSON &GetObject(const string &key);
	//! Gets a ComplexJSON element from the list
	ComplexJSON &GetArrayElement(const idx_t &index);
	//! Gets a string version of the underlying ComplexJSON object from the map
	string GetValue(const string &key) const;
	//! Gets a string version of the underlying ComplexJSON array from the list
	string GetValue(const idx_t &index) const;
	//! Recursive function for GetValue
	static string GetValueRecursive(const ComplexJSON &child);
	//! Flattens this json to a top level key -> nested json
	unordered_map<string, string> Flatten() const;

private:
	//! Basic string value, in case this is the last value of a nested json
	string str_value;
	//! If this is a json object a map of key/value
	unordered_map<string, unique_ptr<ComplexJSON>> obj_value;
	//! If this is a json array a list of values
	vector<unique_ptr<ComplexJSON>> arr_value;
	//! If this json is an object (i.e., map or not)
	ComplexJSONType type;
};

} // namespace goose
