#include <goose/parser/parsed_data/detach_info.h>
#include <goose/parser/keyword_helper.h>

namespace goose {

DetachInfo::DetachInfo() : ParseInfo(TYPE) {
}

unique_ptr<DetachInfo> DetachInfo::Copy() const {
	auto result = make_uniq<DetachInfo>();
	result->name = name;
	result->if_not_found = if_not_found;
	return result;
}

string DetachInfo::ToString() const {
	string result = "";
	result += "DETACH DATABASE";
	if (if_not_found == OnEntryNotFound::RETURN_NULL) {
		result += " IF EXISTS";
	}
	result += " " + KeywordHelper::WriteOptionallyQuoted(name);
	result += ";";
	return result;
}

} // namespace goose
