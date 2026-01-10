include_directories(include)

add_library(goose_linenoise OBJECT highlighting.cc history.cc linenoise.cc
                                    linenoise-c.cc rendering.cc terminal.cc)
set(SHELL_SOURCES
    ${SHELL_SOURCES} $<TARGET_OBJECTS:goose_linenoise>
    PARENT_SCOPE)
