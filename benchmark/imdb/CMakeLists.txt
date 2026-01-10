
add_library(goose_benchmark_imdb OBJECT imdb.cc)
set(BENCHMARK_OBJECT_FILES
    ${BENCHMARK_OBJECT_FILES} $<TARGET_OBJECTS:goose_benchmark_imdb>
    PARENT_SCOPE)
