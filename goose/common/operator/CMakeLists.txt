add_library_unity(goose_common_operators OBJECT cast_operators.cc
                  convert_to_string.cc string_cast.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_common_operators>
    PARENT_SCOPE)
