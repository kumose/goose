#include <goose/common/exception/parser_exception.h>
#include <goose/common/types-import.h>
#include <goose/parser/query_error_context.h>

namespace goose {

ParserException::ParserException(const string &msg) : Exception(ExceptionType::PARSER, msg) {
}

ParserException::ParserException(const unordered_map<string, string> &extra_info, const string &msg)
    : Exception(extra_info, ExceptionType::PARSER, msg) {
}

ParserException ParserException::SyntaxError(const string &query, const string &error_message,
                                             optional_idx error_location) {
	return ParserException(Exception::InitializeExtraInfo("SYNTAX_ERROR", error_location), error_message);
}
} // namespace goose
