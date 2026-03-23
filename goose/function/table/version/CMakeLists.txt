add_definitions(-DGOOSE_SOURCE_ID="\""${GIT_COMMIT_HASH}"\"")
add_definitions(-DGOOSE_VERSION="\""${GOOSE_VERSION}"\"")

add_library_unity(goose_func_table_version OBJECT pragma_version.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_func_table_version>
    PARENT_SCOPE)
