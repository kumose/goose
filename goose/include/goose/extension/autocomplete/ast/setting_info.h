#pragma once

#include <goose/common/enums/set_scope.h>
#include <goose/common/string.h>

namespace goose {

struct SettingInfo {
	string name;
	SetScope scope = SetScope::AUTOMATIC; // Default value is defined here
};

} // namespace goose
