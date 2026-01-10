
add_library(
  goose_benchmark_micro OBJECT append.cc append_mix.cc bulkupdate.cc
                                cast.cc in.cc storage.cc)

set(BENCHMARK_OBJECT_FILES
    ${BENCHMARK_OBJECT_FILES} $<TARGET_OBJECTS:goose_benchmark_micro>
    PARENT_SCOPE)
