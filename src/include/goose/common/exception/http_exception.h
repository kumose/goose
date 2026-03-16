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
