add_library_unity(goose_sample OBJECT base_reservoir_sample.cc
                  reservoir_sample.cc)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:goose_sample>
    PARENT_SCOPE)
