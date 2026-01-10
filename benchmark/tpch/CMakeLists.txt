

set(BENCHMARK_SOURCE_FILES startup.cc)

add_library(goose_benchmark_tpch OBJECT ${BENCHMARK_SOURCE_FILES})

set(BENCHMARK_OBJECT_FILES
    ${BENCHMARK_OBJECT_FILES} $<TARGET_OBJECTS:goose_benchmark_tpch>
    PARENT_SCOPE)
