add_library_unity(goose_csv_state_machine OBJECT csv_state_machine.cc
                  csv_state_machine_cache.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_csv_state_machine>
    PARENT_SCOPE)
