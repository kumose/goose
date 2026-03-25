#include <goose/execution/operator/helper/physical_create_secret.h>
#include <goose/main/client_context.h>
#include <goose/main/database.h>
#include <goose/main/secret/secret_manager.h>

namespace goose {

SourceResultType PhysicalCreateSecret::GetDataInternal(ExecutionContext &context, DataChunk &chunk,
                                                       OperatorSourceInput &input) const {
	auto &client = context.client;
	auto &secret_manager = SecretManager::Get(client);

	secret_manager.CreateSecret(client, create_input);

	chunk.SetValue(0, 0, true);
	chunk.SetCardinality(1);

	return SourceResultType::FINISHED;
}

} // namespace goose
