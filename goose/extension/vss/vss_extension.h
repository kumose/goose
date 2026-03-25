#pragma once

#include <goose/main/extension.h>

namespace goose {

class VssExtension : public Extension {
public:
	void Load(ExtensionLoader &loader) override;
	std::string Name() override;
};

} // namespace goose
