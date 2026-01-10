add_library_unity(test_logging OBJECT test_logging.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_logging>
    PARENT_SCOPE)
