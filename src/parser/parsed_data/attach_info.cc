#include <goose/parser/parsed_data/attach_info.h>
#include <goose/parser/keyword_helper.h>
#include <goose/main/config.h>

namespace goose {

unique_ptr<AttachInfo> AttachInfo::Copy() const {
	auto result = make_uniq<AttachInfo>();
	result->name = name;
	result->path = path;
	result->options = options;
	for (auto &entry : parsed_options) {
		result->parsed_options[entry.first] = entry.second->Copy();
	}
	result->on_conflict = on_conflict;
	return result;
}

string AttachInfo::ToString() const {
	string result = "";
	result += "ATTACH";
	if (on_conflict == OnCreateConflict::IGNORE_ON_CONFLICT) {
		result += " IF NOT EXISTS";
	} else if (on_conflict == OnCreateConflict::REPLACE_ON_CONFLICT) {
		result += " OR REPLACE";
	}
	result += " DATABASE ";
	result += KeywordHelper::WriteQuoted(path, '\'');
	if (!name.empty()) {
		result += " AS " + KeywordHelper::WriteOptionallyQuoted(name);
	}
	if (!parsed_options.empty() || !options.empty()) {
		vector<string> stringified;
		for (auto &opt : parsed_options) {
			stringified.push_back(StringUtil::Format("%s %s", opt.first, opt.second->ToString()));
		}
		for (auto &opt : options) {
			stringified.push_back(StringUtil::Format("%s %s", opt.first, opt.second.ToSQLString()));
		}
		result += " (" + StringUtil::Join(stringified, ", ") + ")";
	}
	result += ";";
	return result;
}

} // namespace goose
