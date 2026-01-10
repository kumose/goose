#include <goose/main/extension_helper.h>

// This is a dummy loader to produce a workable duckdb library without linking any extensions.
// Link this to libgoose_static.a to get a working system.

namespace goose {
void ExtensionHelper::LoadAllExtensions(Goose &db) {
	// nop
}

vector<string> ExtensionHelper::LoadedExtensionTestPaths() {
	return {};
}

} // namespace goose
