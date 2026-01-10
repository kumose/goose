add_library_unity(test_mbedtls OBJECT test_mbedtls.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_mbedtls>
    PARENT_SCOPE)
