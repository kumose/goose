add_library_unity(goose_adbc OBJECT adbc.cc)
add_subdirectory(nanoarrow)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_adbc>
    PARENT_SCOPE)
