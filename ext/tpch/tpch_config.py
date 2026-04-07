import os

# list all include directories
include_directories = [
    os.path.sep.join(x.split('/')) for x in ['extension/tpch/include', 'extension/tpch/dbgen/include']
]
# source files
source_files = [
    os.path.sep.join(x.split('/'))
    for x in [
        'extension/tpch/tpch_extension.cc',
        'extension/tpch/dbgen/bm_utils.cc',
        'extension/tpch/dbgen/build.cc',
        'extension/tpch/dbgen/dbgen.cc',
        'extension/tpch/dbgen/dbgen_gunk.cc',
        'extension/tpch/dbgen/permute.cc',
        'extension/tpch/dbgen/rnd.cc',
        'extension/tpch/dbgen/rng64.cc',
        'extension/tpch/dbgen/speed_seed.cc',
        'extension/tpch/dbgen/text.cc',
    ]
]
