include_directories(include)
include_directories(include/dsdgen-c)
add_subdirectory(dsdgen-c)

add_library(dsdgen OBJECT dsdgen.cc append_info-c.cc dsdgen_helpers.cc)

set(DSDGEN_OBJECT_FILES
    ${DSDGEN_OBJECT_FILES} $<TARGET_OBJECTS:dsdgen>
    PARENT_SCOPE)

disable_target_warnings(dsdgen)
