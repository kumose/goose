add_library_unity(goose_aggr_distr OBJECT count.cc first_last_any.cc
                  minmax.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_aggr_distr>
    PARENT_SCOPE)
