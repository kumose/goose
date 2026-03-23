add_library_unity(goose_main_chunk_scan_state OBJECT query_result.cc
                  batched_data_collection.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_main_chunk_scan_state>
    PARENT_SCOPE)
