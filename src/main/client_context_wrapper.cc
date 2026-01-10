#include <goose/main/client_context_wrapper.h>
#include <goose/main/client_context.h>

namespace goose {

ClientContextWrapper::ClientContextWrapper(const shared_ptr<ClientContext> &context) : client_context(context) {
}

shared_ptr<ClientContext> ClientContextWrapper::TryGetContext() {
	auto actual_context = client_context.lock();
	return actual_context;
}

shared_ptr<ClientContext> ClientContextWrapper::GetContext() {
	auto actual_context = TryGetContext();
	if (!actual_context) {
		throw ConnectionException("Connection has already been closed");
	}
	return actual_context;
}

void ClientContextWrapper::TryBindRelation(Relation &relation, vector<ColumnDefinition> &columns) {
	GetContext()->TryBindRelation(relation, columns);
}

} // namespace goose
