add_library_unity(goose_csv_buffer_manager OBJECT csv_buffer.cc
                  csv_buffer_manager.cc csv_file_handle.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_csv_buffer_manager>
    PARENT_SCOPE)
