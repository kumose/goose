add_library_unity(test_secrets OBJECT test_custom_secret_storage.cc
                  test_persistent_secret_permissions.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:test_secrets>
    PARENT_SCOPE)
