# This file is included by Goose's build system. It specifies which extension to load

# Extension from this repo
goose_extension_load(delta
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}
    LOAD_TESTS
)

# Build the httpfs extension to test with s3/http
goose_extension_load(httpfs)

# Build the tpch and tpcds extension for testing/benchmarking
goose_extension_load(tpch)
goose_extension_load(tpcds)
