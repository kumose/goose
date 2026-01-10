#include <goose/common/tree_renderer.h>

namespace goose {

void TreeRenderer::ToStream(RenderTree &root, std::ostream &ss) {
	if (!UsesRawKeyNames()) {
		root.SanitizeKeyNames();
	}
	return ToStreamInternal(root, ss);
}

} // namespace goose
