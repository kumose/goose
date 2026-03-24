#pragma once

#include <goose/common/case_insensitive_map.h>
#include <goose/common/string.h>

namespace goose {
enum class PEGKeywordCategory : uint8_t {
	KEYWORD_NONE,
	KEYWORD_UNRESERVED,
	KEYWORD_RESERVED,
	KEYWORD_TYPE_FUNC,
	KEYWORD_COL_NAME
};

class PEGKeywordHelper {
public:
	static PEGKeywordHelper &Instance();
	bool KeywordCategoryType(const string &text, PEGKeywordCategory type) const;
	void InitializeKeywordMaps();

private:
	PEGKeywordHelper();
	bool initialized;
	case_insensitive_set_t reserved_keyword_map;
	case_insensitive_set_t unreserved_keyword_map;
	case_insensitive_set_t colname_keyword_map;
	case_insensitive_set_t typefunc_keyword_map;
};
} // namespace goose
