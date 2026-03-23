#include <goose/execution/index/index_type.h>
#include <goose/execution/index/index_type_set.h>
#include <goose/execution/index/art/art.h>

namespace goose {

IndexTypeSet::IndexTypeSet() {
	// Register the ART index type by default
	RegisterIndexType(ART::GetARTIndexType());
}

optional_ptr<IndexType> IndexTypeSet::FindByName(const string &name) {
	lock_guard<mutex> g(lock);
	auto entry = functions.find(name);
	if (entry == functions.end()) {
		return nullptr;
	}
	return &entry->second;
}

void IndexTypeSet::RegisterIndexType(const IndexType &index_type) {
	lock_guard<mutex> g(lock);
	if (functions.find(index_type.name) != functions.end()) {
		throw CatalogException("Index type with name \"%s\" already exists!", index_type.name.c_str());
	}
	functions[index_type.name] = index_type;
}

} // namespace goose
