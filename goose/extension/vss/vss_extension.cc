#include <goose/extension/vss/vss_extension.h>
#include <goose/main/extension/extension_loader.h>

#include <goose/extension/vss/hnsw/hnsw.h>

namespace goose {
    static void LoadInternal(ExtensionLoader &loader) {
        // Register the HNSW index module
        HNSWModule::Register(loader);
    }

    void VssExtension::Load(ExtensionLoader &loader) {
        LoadInternal(loader);
    }

    std::string VssExtension::Name() {
        return "vss";
    }
} // namespace goose

extern "C" {
GOOSE_CPP_EXTENSION_ENTRY(vss, loader) {
    goose::LoadInternal(loader);
}
}
