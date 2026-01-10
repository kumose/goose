add_library_unity(goose_main_secret OBJECT secret.cc secret_manager.cc
                  secret_storage.cc default_secrets.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_main_secret>
    PARENT_SCOPE)
