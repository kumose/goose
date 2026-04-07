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
#include <goose/common/exception.h>
#include <goose/common/types-import.h>

namespace goose {
    class String;
    class ClientContext;
    class DatabaseInstance;
    class TransactionException;

    enum class ErrorType : uint16_t {
        // error message types
        UNSIGNED_EXTENSION = 0,
        INVALIDATED_TRANSACTION = 1,
        INVALIDATED_DATABASE = 2,

        // this should always be the last value
        ERROR_COUNT,
        INVALID = 65535,
    };

    //! The error manager class is responsible for formatting error messages
    //! It allows for error messages to be overridden by extensions and clients
    class ErrorManager {
    public:
        template<typename... ARGS>
        string FormatException(ErrorType error_type, ARGS &&... params) {
            vector<ExceptionFormatValue> values;
            return FormatExceptionRecursive(error_type, values, std::forward<ARGS>(params)...);
        }

        GOOSE_API string FormatExceptionRecursive(ErrorType error_type, vector<ExceptionFormatValue> &values);

        template<class T, typename... ARGS>
        string FormatExceptionRecursive(ErrorType error_type, vector<ExceptionFormatValue> &values, T param,
                                        ARGS &&... params) {
            values.push_back(ExceptionFormatValue::CreateFormatValue<T>(param));
            return FormatExceptionRecursive(error_type, values, std::forward<ARGS>(params)...);
        }

        template<typename... ARGS>
        static string FormatException(ClientContext &context, ErrorType error_type, ARGS &&... params) {
            return Get(context).FormatException(error_type, std::forward<ARGS>(params)...);
        }

        GOOSE_API static InvalidInputException InvalidUnicodeError(const String &input, const string &context);

        GOOSE_API static FatalException InvalidatedDatabase(ClientContext &context, const string &invalidated_msg);

        GOOSE_API static TransactionException InvalidatedTransaction(ClientContext &context);

        //! Adds a custom error for a specific error type
        void AddCustomError(ErrorType type, string new_error);

        GOOSE_API static ErrorManager &Get(ClientContext &context);

        GOOSE_API static ErrorManager &Get(DatabaseInstance &context);

    private:
        map<ErrorType, string> custom_errors;
    };
} // namespace goose
