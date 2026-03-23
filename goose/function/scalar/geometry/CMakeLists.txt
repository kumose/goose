add_library_unity(goose_func_geometry OBJECT geometry_functions.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_geometry>
    PARENT_SCOPE)
