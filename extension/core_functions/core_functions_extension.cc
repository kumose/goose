#include "core_functions_extension.h"
#include "core_functions/function_list.h"

namespace goose {

static void LoadInternal(ExtensionLoader &loader) {
	FunctionList::RegisterExtensionFunctions(loader, CoreFunctionList::GetFunctionList());
}

void CoreFunctionsExtension::Load(ExtensionLoader &loader) {
	LoadInternal(loader);
}

std::string CoreFunctionsExtension::Name() {
	return "core_functions";
}

std::string CoreFunctionsExtension::Version() const {
#ifdef EXT_VERSION_CORE_FUNCTIONS
	return EXT_VERSION_CORE_FUNCTIONS;
#else
	return "";
#endif
}

} // namespace goose

extern "C" {

GOOSE_CPP_EXTENSION_ENTRY(core_functions, loader) {
	goose::LoadInternal(loader);
}
}
