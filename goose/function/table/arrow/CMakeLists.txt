add_library_unity(goose_arrow_conversion
        OBJECT
        arrow_goose_schema.cc
        arrow_array_scan_state.cc
        arrow_type_info.cc
)
set(ALL_OBJECT_FILES
        ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_arrow_conversion>
        PARENT_SCOPE)
