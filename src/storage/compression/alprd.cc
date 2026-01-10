#include <goose/common/limits.h>
#include <goose/function/compression/compression.h>
#include <goose/function/compression_function.h>
#include <goose/storage/compression/alprd/alprd_analyze.h>
#include <goose/storage/compression/alprd/alprd_compress.h>
#include <goose/storage/compression/alprd/alprd_fetch.h>
#include <goose/storage/compression/alprd/alprd_scan.h>

namespace goose {

template <class T>
CompressionFunction GetAlpRDFunction(PhysicalType data_type) {
	throw NotImplementedException("GetAlpFunction not implemented for the given datatype");
}

template <>
CompressionFunction GetAlpRDFunction<float>(PhysicalType data_type) {
	return CompressionFunction(CompressionType::COMPRESSION_ALPRD, data_type, AlpRDInitAnalyze<float>,
	                           AlpRDAnalyze<float>, AlpRDFinalAnalyze<float>, AlpRDInitCompression<float>,
	                           AlpRDCompress<float>, AlpRDFinalizeCompress<float>, AlpRDInitScan<float>,
	                           AlpRDScan<float>, AlpRDScanPartial<float>, AlpRDFetchRow<float>, AlpRDSkip<float>);
}

template <>
CompressionFunction GetAlpRDFunction<double>(PhysicalType data_type) {
	return CompressionFunction(CompressionType::COMPRESSION_ALPRD, data_type, AlpRDInitAnalyze<double>,
	                           AlpRDAnalyze<double>, AlpRDFinalAnalyze<double>, AlpRDInitCompression<double>,
	                           AlpRDCompress<double>, AlpRDFinalizeCompress<double>, AlpRDInitScan<double>,
	                           AlpRDScan<double>, AlpRDScanPartial<double>, AlpRDFetchRow<double>, AlpRDSkip<double>);
}

CompressionFunction AlpRDCompressionFun::GetFunction(PhysicalType type) {
	switch (type) {
	case PhysicalType::FLOAT:
		return GetAlpRDFunction<float>(type);
	case PhysicalType::DOUBLE:
		return GetAlpRDFunction<double>(type);
	default:
		throw InternalException("Unsupported type for Alp");
	}
}

bool AlpRDCompressionFun::TypeIsSupported(const PhysicalType physical_type) {
	switch (physical_type) {
	case PhysicalType::FLOAT:
	case PhysicalType::DOUBLE:
		return true;
	default:
		return false;
	}
}

} // namespace goose
