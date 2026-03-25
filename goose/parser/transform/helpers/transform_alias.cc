#include <goose/parser/transformer.h>

namespace goose {

vector<string> Transformer::TransformStringList(cantor::PGList *list) {
	vector<string> result;
	if (!list) {
		return result;
	}
	for (auto node = list->head; node != nullptr; node = node->next) {
		auto value = PGPointerCast<cantor::PGValue>(node->data.ptr_value);
		result.emplace_back(value->val.str);
	}
	return result;
}

string Transformer::TransformAlias(cantor::PGAlias *root, vector<string> &column_name_alias) {
	if (!root) {
		return "";
	}
	column_name_alias = TransformStringList(root->colnames);
	return root->aliasname;
}

} // namespace goose
