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
