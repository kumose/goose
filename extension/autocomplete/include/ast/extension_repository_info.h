#pragma once
#include <goose/common/string.h>

namespace goose {
struct ExtensionRepositoryInfo {
	string name;
	bool repository_is_alias = false;
};
} // namespace goose
