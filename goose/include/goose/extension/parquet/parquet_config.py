import os

# list all include directories
include_directories = [
    os.path.sep.join(x.split('/'))
    for x in [
        'extension/parquet/include',
        'third_party/parquet',
        'third_party/thrift',
        'third_party/brotli/include',
        'third_party/brotli/common',
        'third_party/brotli/dec',
        'third_party/brotli/enc',
        'third_party/snappy',
        'third_party/mbedtls',
        'third_party/mbedtls/include',
        'third_party/zstd/include',
    ]
]
prefix = os.path.join('extension', 'parquet')


def list_files_recursive(rootdir, suffix):
    file_list = []
    for root, _, files in os.walk(rootdir):
        file_list += [os.path.join(root, f) for f in files if f.endswith(suffix)]
    return file_list


source_files = list_files_recursive(prefix, '.cc')

# parquet/thrift/snappy
source_files += [
    os.path.sep.join(x.split('/'))
    for x in [
        'third_party/parquet/parquet_types.cc',
        'third_party/thrift/kthrift/protocol/TProtocol.cc',
        'third_party/thrift/kthrift/transport/TTransportException.cc',
        'third_party/thrift/kthrift/transport/TBufferTransports.cc',
        'third_party/snappy/snappy.cc',
        'third_party/snappy/snappy-sinksource.cc',
    ]
]

# brotli
source_files += [
    os.path.sep.join(x.split('/'))
    for x in [
        'third_party/brotli/common/constants.cc',
        'third_party/brotli/common/context.cc',
        'third_party/brotli/common/dictionary.cc',
        'third_party/brotli/common/platform.cc',
        'third_party/brotli/common/shared_dictionary.cc',
        'third_party/brotli/common/transform.cc',
        'third_party/brotli/dec/bit_reader.cc',
        'third_party/brotli/dec/decode.cc',
        'third_party/brotli/dec/huffman.cc',
        'third_party/brotli/dec/state.cc',
        'third_party/brotli/enc/backward_references.cc',
        'third_party/brotli/enc/backward_references_hq.cc',
        'third_party/brotli/enc/bit_cost.cc',
        'third_party/brotli/enc/block_splitter.cc',
        'third_party/brotli/enc/brotli_bit_stream.cc',
        'third_party/brotli/enc/cluster.cc',
        'third_party/brotli/enc/command.cc',
        'third_party/brotli/enc/compound_dictionary.cc',
        'third_party/brotli/enc/compress_fragment.cc',
        'third_party/brotli/enc/compress_fragment_two_pass.cc',
        'third_party/brotli/enc/dictionary_hash.cc',
        'third_party/brotli/enc/encode.cc',
        'third_party/brotli/enc/encoder_dict.cc',
        'third_party/brotli/enc/entropy_encode.cc',
        'third_party/brotli/enc/fast_log.cc',
        'third_party/brotli/enc/histogram.cc',
        'third_party/brotli/enc/literal_cost.cc',
        'third_party/brotli/enc/memory.cc',
        'third_party/brotli/enc/metablock.cc',
        'third_party/brotli/enc/static_dict.cc',
        'third_party/brotli/enc/utf8_util.cc',
    ]
]
