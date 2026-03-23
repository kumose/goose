add_library_unity(
  goose_operator_projection OBJECT physical_projection.cc
  physical_tableinout_function.cc physical_pivot.cc physical_unnest.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_operator_projection>
    PARENT_SCOPE)
