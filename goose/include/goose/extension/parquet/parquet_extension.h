//===----------------------------------------------------------------------===//
//                         Goose
//
// parquet_extension.h
//
//
//===----------------------------------------------------------------------===/

#pragma once

#include <goose/goose.h>

namespace goose {

class ParquetExtension : public Extension {
public:
	void Load(ExtensionLoader &loader) override;
	std::string Name() override;
	std::string Version() const override;
};

} // namespace goose
