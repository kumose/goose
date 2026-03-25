#include <goose/parser/tableref/emptytableref.h>

namespace goose {

string EmptyTableRef::ToString() const {
	return "";
}

bool EmptyTableRef::Equals(const TableRef &other) const {
	return TableRef::Equals(other);
}

unique_ptr<TableRef> EmptyTableRef::Copy() {
	return make_uniq<EmptyTableRef>();
}

} // namespace goose
