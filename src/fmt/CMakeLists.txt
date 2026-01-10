
add_library_unity(
        goosedb_fmt
        OBJECT
        format.cc
)
set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goosedb_fmt>
        PARENT_SCOPE)