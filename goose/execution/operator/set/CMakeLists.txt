add_library_unity(goose_operator_set OBJECT physical_union.cc
                  physical_recursive_cte.cc physical_cte.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_operator_set>
    PARENT_SCOPE)
