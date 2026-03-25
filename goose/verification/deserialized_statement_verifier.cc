#include <goose/verification/deserialized_statement_verifier.h>

#include <goose/common/serializer/binary_deserializer.h>
#include <goose/common/serializer/binary_serializer.h>
#include <goose/common/serializer/memory_stream.h>

namespace goose {
    DeserializedStatementVerifier::DeserializedStatementVerifier(
        unique_ptr<SQLStatement> statement_p, optional_ptr<case_insensitive_map_t<BoundParameterData> > parameters)
        : StatementVerifier(VerificationType::DESERIALIZED, "Deserialized", std::move(statement_p), parameters) {
    }

    unique_ptr<StatementVerifier>
    DeserializedStatementVerifier::Create(const SQLStatement &statement,
                                          optional_ptr<case_insensitive_map_t<BoundParameterData> > parameters) {
        auto &select_stmt = statement.Cast<SelectStatement>();
        Allocator allocator;
        MemoryStream stream(allocator);
        SerializationOptions options;
        options.serialization_compatibility = SerializationCompatibility::FromString("latest");
        BinarySerializer::Serialize(select_stmt, stream, options);
        stream.Rewind();
        auto result = BinaryDeserializer::Deserialize<SelectStatement>(stream);

        return make_uniq < DeserializedStatementVerifier > (std::move(result), parameters);
    }
} // namespace goose