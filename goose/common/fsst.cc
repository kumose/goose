// Copyright (c) 2025 DuckDB.
// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//


#include <goose/common/fsst.h>

namespace goose {
    string FSSTPrimitives::DecompressValue(void *goose_fsst_decoder, const char *compressed_string,
                                           const idx_t compressed_string_len,
                                           vector<unsigned char> &decompress_buffer) {
        auto compressed_string_ptr = reinterpret_cast<const unsigned char *>(compressed_string);
        auto fsst_decoder = static_cast<xfsst_decoder_t *>(goose_fsst_decoder);
        auto decompressed_string_size = xfsst_decompress(fsst_decoder, compressed_string_len, compressed_string_ptr,
                                                         decompress_buffer.size(), decompress_buffer.data());

        D_ASSERT(!decompress_buffer.empty());
        D_ASSERT(decompressed_string_size <= decompress_buffer.size() - 1);
        return string(char_ptr_cast(decompress_buffer.data()), decompressed_string_size);
    }
} // namespace goose
