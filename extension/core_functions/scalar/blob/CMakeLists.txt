add_library_unity(goose_core_functions_blob OBJECT base64.cc encode.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_blob>
    PARENT_SCOPE)
