#include "delta_extension.hpp"
#include "delta_functions.hpp"

#include <goose/goose.h>
#include <goose/common/exception.h>
#include <goose/main/extension/extension_loader.h>

namespace goose {

static void LoadInternal(ExtensionLoader &loader) {
	// Load functions
	for (const auto &function : DeltaFunctions::GetTableFunctions(instance)) {
		loader.RegisterFunction(function);
	}
}

void DeltaExtension::Load(ExtensionLoader &loader) {
	LoadInternal(loader);
}

std::string DeltaExtension::Name() {
	return "delta";
}

} // namespace goose

extern "C" {

GOOSE_CPP_EXTENSION_ENTRY(delta, loader) {
	goose::LoadInternal(loader);
}
}
