add_library_unity(goose_main_buffered_data OBJECT buffered_data.cc
                  simple_buffered_data.cc batched_buffered_data.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_main_buffered_data>
    PARENT_SCOPE)
