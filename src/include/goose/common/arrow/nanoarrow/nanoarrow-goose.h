#pragma once

#include "goose/common/arrow/nanoarrow/nanoarrow.h"

// Bring in the symbols from goose_nanoarrow into goose
namespace goose {

// using goose_nanoarrow::ArrowBuffer; //We have a variant of this that should be renamed
using goose_nanoarrow::ArrowBufferAllocator;
using goose_nanoarrow::ArrowError;
using goose_nanoarrow::ArrowSchemaView;
using goose_nanoarrow::ArrowStringView;

} // namespace goose
