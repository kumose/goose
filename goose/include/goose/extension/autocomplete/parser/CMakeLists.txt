add_library_unity(goose_peg_parser OBJECT peg_parser.cc)
set(AUTOCOMPLETE_EXTENSION_FILES
    ${AUTOCOMPLETE_EXTENSION_FILES} $<TARGET_OBJECTS:goose_peg_parser>
    PARENT_SCOPE)
