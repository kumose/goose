add_library_unity(
        goosedb_utility
        OBJECT
        yyjson.cc
        utf8proc.cc
        utf8proc_wrapper.cc
        hyperloglog.cc
        sds.cc
        skip_list.cc
        bitpacking.cc
        miniz.cc
        mbedtls_wrapper.cc
        xxhash.cc
)
set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goosedb_utility>
        PARENT_SCOPE)
