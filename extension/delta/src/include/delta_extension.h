#pragma once

#include <goose/goose.h>

namespace goose {

class DeltaExtension : public Extension {
public:
	void Load(ExtensionLoader &loader) override;
	std::string Name() override;
};

} // namespace goose
