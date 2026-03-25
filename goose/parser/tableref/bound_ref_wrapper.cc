#include <goose/parser/tableref/bound_ref_wrapper.h>

namespace goose {

BoundRefWrapper::BoundRefWrapper(BoundStatement bound_ref_p, shared_ptr<Binder> binder_p)
    : TableRef(TableReferenceType::BOUND_TABLE_REF), bound_ref(std::move(bound_ref_p)), binder(std::move(binder_p)) {
}

string BoundRefWrapper::ToString() const {
	throw InternalException("Method not implemented for BoundRefWrapper");
}

bool BoundRefWrapper::Equals(const TableRef &other_p) const {
	throw InternalException("Method not implemented for BoundRefWrapper");
}

unique_ptr<TableRef> BoundRefWrapper::Copy() {
	throw InternalException("Method not implemented for BoundRefWrapper");
}

void BoundRefWrapper::Serialize(Serializer &serializer) const {
	throw InternalException("Method not implemented for BoundRefWrapper");
}

unique_ptr<TableRef> BoundRefWrapper::Deserialize(Deserializer &source) {
	throw InternalException("Method not implemented for BoundRefWrapper");
}

} // namespace goose
