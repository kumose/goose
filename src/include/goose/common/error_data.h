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

#include <goose/common/exception.h>
#include <goose/common/string.h>

namespace goose {
class ParsedExpression;
class TableRef;

class ErrorData {
public:
	//! Not initialized, default constructor
	GOOSE_API ErrorData();
	//! From std::exception
	GOOSE_API ErrorData(const std::exception &ex); // NOLINT: allow implicit construction from exception
	//! From a raw string and exception type
	GOOSE_API ErrorData(ExceptionType type, const string &raw_message);
	//! From a raw string
	GOOSE_API explicit ErrorData(const string &raw_message);

public:
	//! Throw the error
	[[noreturn]] GOOSE_API void Throw(const string &prepended_message = "") const;
	//! Get the internal exception type of the error.
	GOOSE_API const ExceptionType &Type() const;
	//! Used in clients like C-API, creates the final message and returns a reference to it
	GOOSE_API const string &Message() const {
		return final_message;
	}
	GOOSE_API const string &RawMessage() const {
		return raw_message;
	}
	GOOSE_API void Merge(const ErrorData &other);
	GOOSE_API bool operator==(const ErrorData &other) const;

	//! Returns true, if this error data contains an exception, else false.
	inline bool HasError() const {
		return initialized;
	}
	const unordered_map<string, string> &ExtraInfo() const {
		return extra_info;
	}

	GOOSE_API void FinalizeError();
	GOOSE_API void AddErrorLocation(const string &query);
	GOOSE_API void ConvertErrorToJSON();

	GOOSE_API void AddQueryLocation(optional_idx query_location);
	GOOSE_API void AddQueryLocation(QueryErrorContext error_context);
	GOOSE_API void AddQueryLocation(const ParsedExpression &ref);
	GOOSE_API void AddQueryLocation(const TableRef &ref);

private:
	//! Whether this ErrorData contains an exception or not
	bool initialized;
	//! The ExceptionType of the preserved exception
	ExceptionType type;
	//! The message the exception was constructed with (does not contain the Exception Type)
	string raw_message;
	//! The final message (stored in the preserved error for compatibility reasons with C-API)
	string final_message;
	//! Extra exception info
	unordered_map<string, string> extra_info;

private:
	GOOSE_API static string SanitizeErrorMessage(string error);
	GOOSE_API string ConstructFinalMessage() const;
};

} // namespace goose
