#!/bin/bash
rm -rf gen-cpp
thrift --gen "cpp:moveable_types,no_default_operators=true" parquet.thrift
cp gen-cpp/* .
sed -i .bak -e "s/std::vector/goose::vector/" parquet_types.*
sed -i .bak -e "s/namespace parquet/namespace goose_parquet/" parquet_types.*
sed -i .bak -e 's/namespace goose_parquet {/#include <goose/extension/parquet/windows_compatibility.h>\nnamespace apache = goose_apache;\n\nnamespace goose_parquet {/' parquet_types.h
sed -i .bak '/namespace goose_parquet {/a\
\
template <class ENUM>\
static typename ENUM::type SafeEnumCast(const std::map<int, const char*> &values_to_names, const int &ecast) {\
  if (values_to_names.find(ecast) == values_to_names.end()) {\
    throw goose_apache::thrift::protocol::TProtocolException(goose_apache::thrift::protocol::TProtocolException::INVALID_DATA);\
  }\
  return static_cast<typename ENUM::type>(ecast);\
}\
' parquet_types.cc
sed -i .bak -e 's/static_cast<Type::type>(/SafeEnumCast<Type>(_Type_VALUES_TO_NAMES, /' parquet_types.*
sed -i .bak -e 's/static_cast<ConvertedType::type>(/SafeEnumCast<ConvertedType>(_ConvertedType_VALUES_TO_NAMES, /' parquet_types.*
sed -i .bak -e 's/static_cast<FieldRepetitionType::type>(/SafeEnumCast<FieldRepetitionType>(_FieldRepetitionType_VALUES_TO_NAMES, /' parquet_types.*
sed -i .bak -e 's/static_cast<EdgeInterpolationAlgorithm::type>(/SafeEnumCast<EdgeInterpolationAlgorithm>(_EdgeInterpolationAlgorithm_VALUES_TO_NAMES, /' parquet_types.*
sed -i .bak -e 's/static_cast<Encoding::type>(/SafeEnumCast<Encoding>(_Encoding_VALUES_TO_NAMES, /' parquet_types.*
sed -i .bak -e 's/static_cast<CompressionCodec::type>(/SafeEnumCast<CompressionCodec>(_CompressionCodec_VALUES_TO_NAMES, /' parquet_types.*
sed -i .bak -e 's/static_cast<PageType::type>(/SafeEnumCast<PageType>(_PageType_VALUES_TO_NAMES, /' parquet_types.*
sed -i .bak -e 's/static_cast<BoundaryOrder::type>(/SafeEnumCast<BoundaryOrder>(_BoundaryOrder_VALUES_TO_NAMES, /' parquet_types.*
rm *.bak
rm -rf gen-cpp