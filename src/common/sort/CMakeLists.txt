add_library_unity(
  goose_sort
  OBJECT
  full_sort.cc
  hashed_sort.cc
  natural_sort.cc
  sort.cc
  sort_strategy.cc
  sorted_run.cc
  sorted_run_merger.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_sort>
    PARENT_SCOPE)
