add_library_unity(goose_main_capi_cast OBJECT utils-c.cc from_decimal-c.cc)

set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_main_capi_cast>
    PARENT_SCOPE)
