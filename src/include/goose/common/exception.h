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

#include <goose/common/assert.h>
#include <goose/common/exception_format_value.h>
#include <goose/common/types-import.h>
#include <goose/common/typedefs.h>

#include <vector>
#include <stdexcept>

namespace goose {
enum class PhysicalType : uint8_t;
struct LogicalType;
class Expression;
class ParsedExpression;
class QueryErrorContext;
class TableRef;
struct hugeint_t;
class optional_idx; // NOLINT: matching std style

inline void AssertRestrictFunction(const void *left_start, const void *left_end, const void *right_start,
                                   const void *right_end, const char *fname, int linenr) {
	// assert that the two pointers do not overlap
#ifdef DEBUG
	if (!(left_end <= right_start || right_end <= left_start)) {
		printf("ASSERT RESTRICT FAILED: %s:%d\n", fname, linenr);
		D_ASSERT(0);
	}
#endif
}

#define ASSERT_RESTRICT(left_start, left_end, right_start, right_end)                                                  \
	AssertRestrictFunction(left_start, left_end, right_start, right_end, __FILE__, __LINE__)

//===--------------------------------------------------------------------===//
// Exception Types
//===--------------------------------------------------------------------===//

enum class ExceptionType : uint8_t {
	INVALID = 0,          // invalid type
	OUT_OF_RANGE = 1,     // value out of range error
	CONVERSION = 2,       // conversion/casting error
	UNKNOWN_TYPE = 3,     // unknown type
	DECIMAL = 4,          // decimal related
	MISMATCH_TYPE = 5,    // type mismatch
	DIVIDE_BY_ZERO = 6,   // divide by 0
	OBJECT_SIZE = 7,      // object size exceeded
	INVALID_TYPE = 8,     // incompatible for operation
	SERIALIZATION = 9,    // serialization
	TRANSACTION = 10,     // transaction management
	NOT_IMPLEMENTED = 11, // method not implemented
	EXPRESSION = 12,      // expression parsing
	CATALOG = 13,         // catalog related
	PARSER = 14,          // parser related
	PLANNER = 15,         // planner related
	SCHEDULER = 16,       // scheduler related
	EXECUTOR = 17,        // executor related
	CONSTRAINT = 18,      // constraint related
	INDEX = 19,           // index related
	STAT = 20,            // stat related
	CONNECTION = 21,      // connection related
	SYNTAX = 22,          // syntax related
	SETTINGS = 23,        // settings related
	BINDER = 24,          // binder related
	NETWORK = 25,         // network related
	OPTIMIZER = 26,       // optimizer related
	NULL_POINTER = 27,    // nullptr exception
	IO = 28,              // IO exception
	INTERRUPT = 29,       // interrupt
	FATAL = 30,           // Fatal exceptions are non-recoverable, and render the entire DB in an unusable state
	INTERNAL = 31,        // Internal exceptions indicate something went wrong internally (i.e. bug in the code base)
	INVALID_INPUT = 32,   // Input or arguments error
	OUT_OF_MEMORY = 33,   // out of memory
	PERMISSION = 34,      // insufficient permissions
	PARAMETER_NOT_RESOLVED = 35, // parameter types could not be resolved
	PARAMETER_NOT_ALLOWED = 36,  // parameter types not allowed
	DEPENDENCY = 37,             // dependency
	HTTP = 38,
	MISSING_EXTENSION = 39, // Thrown when an extension is used but not loaded
	AUTOLOAD = 40,          // Thrown when an extension is used but not loaded
	SEQUENCE = 41,
	INVALID_CONFIGURATION =
	    42 // An invalid configuration was detected (e.g. a Secret param was missing, or a required setting not found)
};

class Exception : public std::runtime_error {
public:
	GOOSE_API Exception(ExceptionType exception_type, const string &message);

	GOOSE_API Exception(const unordered_map<string, string> &extra_info, ExceptionType exception_type,
	                     const string &message);

public:
	GOOSE_API static string ExceptionTypeToString(ExceptionType type);
	GOOSE_API static ExceptionType StringToExceptionType(const string &type);

	template <typename... ARGS>
	static string ConstructMessage(const string &msg, ARGS const &...params) {
		const std::size_t num_args = sizeof...(ARGS);
		if (num_args == 0) {
			return msg;
		}
		std::vector<ExceptionFormatValue> values;
		return ConstructMessageRecursive(msg, values, params...);
	}

	GOOSE_API static unordered_map<string, string> InitializeExtraInfo(const Expression &expr);
	GOOSE_API static unordered_map<string, string> InitializeExtraInfo(const ParsedExpression &expr);
	GOOSE_API static unordered_map<string, string> InitializeExtraInfo(const QueryErrorContext &error_context);
	GOOSE_API static unordered_map<string, string> InitializeExtraInfo(const TableRef &ref);
	GOOSE_API static unordered_map<string, string> InitializeExtraInfo(optional_idx error_location);
	GOOSE_API static unordered_map<string, string> InitializeExtraInfo(const string &subtype,
	                                                                    optional_idx error_location);

	//! Whether this exception type can occur during execution of a query
	GOOSE_API static bool IsExecutionError(ExceptionType type);
	GOOSE_API static string ToJSON(ExceptionType type, const string &message);

	GOOSE_API static string ToJSON(const unordered_map<string, string> &extra_info, ExceptionType type,
	                                const string &message);

	GOOSE_API static bool InvalidatesTransaction(ExceptionType exception_type);
	GOOSE_API static bool InvalidatesDatabase(ExceptionType exception_type);

	GOOSE_API static string ConstructMessageRecursive(const string &msg, std::vector<ExceptionFormatValue> &values);

	template <class T, typename... ARGS>
	static string ConstructMessageRecursive(const string &msg, std::vector<ExceptionFormatValue> &values,
	                                        const T &param, ARGS &&...params) {
		values.push_back(ExceptionFormatValue::CreateFormatValue<T>(param));
		return ConstructMessageRecursive(msg, values, params...);
	}

	GOOSE_API static bool UncaughtException();

	GOOSE_API static string GetStackTrace(idx_t max_depth = 120);
	static string FormatStackTrace(const string &message = "") {
		return (message + "\n" + GetStackTrace());
	}

	GOOSE_API static void SetQueryLocation(optional_idx error_location, unordered_map<string, string> &extra_info);
};

//===--------------------------------------------------------------------===//
// Exception derived classes
//===--------------------------------------------------------------------===//
class ConnectionException : public Exception {
public:
	GOOSE_API explicit ConnectionException(const string &msg);

	template <typename... ARGS>
	explicit ConnectionException(const string &msg, ARGS &&...params)
	    : ConnectionException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class PermissionException : public Exception {
public:
	GOOSE_API explicit PermissionException(const string &msg);

	template <typename... ARGS>
	explicit PermissionException(const string &msg, ARGS &&...params)
	    : PermissionException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class OutOfRangeException : public Exception {
public:
	GOOSE_API explicit OutOfRangeException(const string &msg);

	template <typename... ARGS>
	explicit OutOfRangeException(const string &msg, ARGS &&...params)
	    : OutOfRangeException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
	GOOSE_API OutOfRangeException(const int64_t value, const PhysicalType orig_type, const PhysicalType new_type);
	GOOSE_API OutOfRangeException(const hugeint_t value, const PhysicalType orig_type, const PhysicalType new_type);
	GOOSE_API OutOfRangeException(const double value, const PhysicalType orig_type, const PhysicalType new_type);
	GOOSE_API OutOfRangeException(const PhysicalType var_type, const idx_t length);
};

class OutOfMemoryException : public Exception {
public:
	GOOSE_API explicit OutOfMemoryException(const string &msg);

	template <typename... ARGS>
	explicit OutOfMemoryException(const string &msg, ARGS &&...params)
	    : OutOfMemoryException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

private:
	string ExtendOutOfMemoryError(const string &msg);
};

class SyntaxException : public Exception {
public:
	GOOSE_API explicit SyntaxException(const string &msg);

	template <typename... ARGS>
	explicit SyntaxException(const string &msg, ARGS &&...params)
	    : SyntaxException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class ConstraintException : public Exception {
public:
	GOOSE_API explicit ConstraintException(const string &msg);

	template <typename... ARGS>
	explicit ConstraintException(const string &msg, ARGS &&...params)
	    : ConstraintException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class DependencyException : public Exception {
public:
	GOOSE_API explicit DependencyException(const string &msg);

	template <typename... ARGS>
	explicit DependencyException(const string &msg, ARGS &&...params)
	    : DependencyException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class IOException : public Exception {
public:
	GOOSE_API explicit IOException(const string &msg);

	GOOSE_API explicit IOException(const unordered_map<string, string> &extra_info, const string &msg);
	explicit IOException(ExceptionType exception_type, const string &msg) : Exception(exception_type, msg) {
	}

	template <typename... ARGS>
	explicit IOException(const string &msg, ARGS &&...params)
	    : IOException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	template <typename... ARGS>
	explicit IOException(const unordered_map<string, string> &extra_info, const string &msg, ARGS &&...params)
	    : IOException(extra_info, ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class MissingExtensionException : public Exception {
public:
	GOOSE_API explicit MissingExtensionException(const string &msg);

	template <typename... ARGS>
	explicit MissingExtensionException(const string &msg, ARGS &&...params)
	    : MissingExtensionException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class NotImplementedException : public Exception {
public:
	GOOSE_API explicit NotImplementedException(const string &msg);
	explicit NotImplementedException(const unordered_map<string, string> &extra_info, const string &msg);

	template <typename... ARGS>
	explicit NotImplementedException(const string &msg, ARGS &&...params)
	    : NotImplementedException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
	template <typename... ARGS>
	explicit NotImplementedException(const unordered_map<string, string> &extra_info, const string &msg,
	                                 ARGS &&...params)
	    : NotImplementedException(extra_info, ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class AutoloadException : public Exception {
public:
	GOOSE_API explicit AutoloadException(const string &extension_name, const string &message);
};

class SerializationException : public Exception {
public:
	GOOSE_API explicit SerializationException(const string &msg);

	template <typename... ARGS>
	explicit SerializationException(const string &msg, ARGS &&...params)
	    : SerializationException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class SequenceException : public Exception {
public:
	GOOSE_API explicit SequenceException(const string &msg);

	template <typename... ARGS>
	explicit SequenceException(const string &msg, ARGS &&...params)
	    : SequenceException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class InterruptException : public Exception {
public:
	GOOSE_API InterruptException();
};

class FatalException : public Exception {
public:
	explicit FatalException(const string &msg) : FatalException(ExceptionType::FATAL, msg) {
	}
	template <typename... ARGS>
	explicit FatalException(const string &msg, ARGS &&...params)
	    : FatalException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

protected:
	GOOSE_API explicit FatalException(ExceptionType type, const string &msg);
	template <typename... ARGS>
	explicit FatalException(ExceptionType type, const string &msg, ARGS &&...params)
	    : FatalException(type, ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class InternalException : public Exception {
public:
	GOOSE_API explicit InternalException(const string &msg);
	InternalException(const unordered_map<string, string> &extra_info, const string &msg);

	template <typename... ARGS>
	explicit InternalException(const string &msg, ARGS &&...params)
	    : InternalException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	template <typename... ARGS>
	explicit InternalException(const unordered_map<string, string> &extra_info, const string &msg, ARGS &&...params)
	    : InternalException(extra_info, ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class InvalidInputException : public Exception {
public:
	GOOSE_API explicit InvalidInputException(const string &msg);
	GOOSE_API explicit InvalidInputException(const unordered_map<string, string> &extra_info, const string &msg);

	template <typename... ARGS>
	explicit InvalidInputException(const string &msg, ARGS &&...params)
	    : InvalidInputException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}

	template <typename... ARGS>
	explicit InvalidInputException(const Expression &expr, const string &msg, ARGS &&...params)
	    : InvalidInputException(Exception::InitializeExtraInfo(expr),
	                            ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class ExecutorException : public Exception {
public:
	GOOSE_API explicit ExecutorException(const string &msg);

	template <typename... ARGS>
	explicit ExecutorException(const string &msg, ARGS &&...params)
	    : ExecutorException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

class InvalidConfigurationException : public Exception {
public:
	GOOSE_API explicit InvalidConfigurationException(const string &msg);

	GOOSE_API explicit InvalidConfigurationException(const unordered_map<string, string> &extra_info,
	                                                  const string &msg);

	template <typename... ARGS>
	explicit InvalidConfigurationException(const string &msg, ARGS &&...params)
	    : InvalidConfigurationException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
	template <typename... ARGS>
	explicit InvalidConfigurationException(const Expression &expr, const string &msg, ARGS &&...params)
	    : InvalidConfigurationException(ConstructMessage(msg, std::forward<ARGS>(params)...),
	                                    Exception::InitializeExtraInfo(expr)) {
	}
};

class InvalidTypeException : public Exception {
public:
	GOOSE_API InvalidTypeException(PhysicalType type, const string &msg);
	GOOSE_API InvalidTypeException(const LogicalType &type, const string &msg);
	GOOSE_API explicit InvalidTypeException(const string &msg);
};

class TypeMismatchException : public Exception {
public:
	GOOSE_API TypeMismatchException(const PhysicalType type_1, const PhysicalType type_2, const string &msg);
	GOOSE_API TypeMismatchException(const LogicalType &type_1, const LogicalType &type_2, const string &msg);
	GOOSE_API TypeMismatchException(optional_idx error_location, const LogicalType &type_1, const LogicalType &type_2,
	                                 const string &msg);
	GOOSE_API explicit TypeMismatchException(const string &msg);
};

class ParameterNotAllowedException : public Exception {
public:
	GOOSE_API explicit ParameterNotAllowedException(const string &msg);

	template <typename... ARGS>
	explicit ParameterNotAllowedException(const string &msg, ARGS &&...params)
	    : ParameterNotAllowedException(ConstructMessage(msg, std::forward<ARGS>(params)...)) {
	}
};

//! Special exception that should be thrown in the binder if parameter types could not be resolved
//! This will cause prepared statements to be forcibly rebound with the actual parameter values
//! This exception is fatal if thrown outside of the binder (i.e. it should never be thrown outside of the binder)
class ParameterNotResolvedException : public Exception {
public:
	GOOSE_API explicit ParameterNotResolvedException();
};

} // namespace goose
