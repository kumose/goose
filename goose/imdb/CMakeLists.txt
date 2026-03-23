add_library_unity(
        goosedb_imdb
        OBJECT
        imdb.cc
)
set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goosedb_imdb>
        PARENT_SCOPE)
