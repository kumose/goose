#include <goose/function/compression/compression.h>
#include <goose/storage/compression/empty_validity.h>

namespace goose {

CompressionFunction EmptyValidityCompressionFun::GetFunction(PhysicalType type) {
	return EmptyValidityCompression::CreateFunction();
}

bool EmptyValidityCompressionFun::TypeIsSupported(const PhysicalType physical_type) {
	return physical_type == PhysicalType::BIT;
}

} // namespace goose
