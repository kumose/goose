add_library_unity(goose_row_operations OBJECT row_aggregate.cc
                  row_matcher.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_row_operations>
    PARENT_SCOPE)
