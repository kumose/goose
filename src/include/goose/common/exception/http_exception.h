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
#include <goose/common/types-import.h>

namespace goose {

class HTTPException : public Exception {
public:
	template <typename>
	struct ResponseShape {
		typedef int status; // NOLINT
	};

	explicit HTTPException(const string &message) : Exception(ExceptionType::HTTP, message) {
	}

	template <class RESPONSE, typename ResponseShape<decltype(RESPONSE::status)>::status = 0, typename... ARGS>
	explicit HTTPException(RESPONSE &response, const string &msg, ARGS &&...params)
	    : HTTPException(static_cast<int>(response.status), response.body, response.headers, response.reason, msg,
	                    std::forward<ARGS>(params)...) {
	}

	template <typename>
	struct ResponseWrapperShape {
		typedef int code; // NOLINT
	};

	template <class RESPONSE, typename ResponseWrapperShape<decltype(RESPONSE::code)>::code = 0, typename... ARGS>
	explicit HTTPException(RESPONSE &response, const string &msg, ARGS &&...params)
	    : HTTPException(static_cast<int>(response.code), response.body, response.headers, response.error, msg,
	                    std::forward<ARGS>(params)...) {
	}

	template <class HEADERS, typename... ARGS>
	explicit HTTPException(int status_code, const string &response_body, const HEADERS &headers, const string &reason,
	                       const string &msg, ARGS &&...params)
	    : Exception(HTTPExtraInfo(status_code, response_body, headers, reason), ExceptionType::HTTP,
	                ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	template <class HEADERS>
	static unordered_map<string, string> HTTPExtraInfo(int status_code, const string &response_body,
	                                                   const HEADERS &headers, const string &reason) {
		unordered_map<string, string> extra_info;
		extra_info["status_code"] = to_string(status_code);
		extra_info["reason"] = reason;
		extra_info["response_body"] = response_body;
		for (auto &entry : headers) {
			extra_info["header_" + entry.first] = entry.second;
		}
		return extra_info;
	}
};

} // namespace goose
