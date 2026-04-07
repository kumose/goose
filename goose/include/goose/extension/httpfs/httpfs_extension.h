#pragma once

#include <goose/goose.h>

namespace goose {
    class HttpfsExtension : public Extension {
    public:
        void Load(ExtensionLoader &loader) override;

        std::string Name() override;

        std::string Version() const override;
    };
} // namespace goose