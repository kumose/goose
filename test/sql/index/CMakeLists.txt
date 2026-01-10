add_library_unity(test_index OBJECT test_art_index.cc test_art_keys.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_index>
    PARENT_SCOPE)
