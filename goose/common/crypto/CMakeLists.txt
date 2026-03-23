add_library_unity(goose_common_crypto OBJECT md5.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_crypto>
    PARENT_SCOPE)
