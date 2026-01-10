add_library_unity(test_persistence OBJECT test_file_matches_wal.cc
                  test_locking.cc test_persistence.cc test_sequence_crash.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_persistence>
    PARENT_SCOPE)
