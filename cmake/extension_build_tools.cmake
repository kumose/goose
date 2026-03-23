# Copyright (C) Kumo inc. and its affiliates.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


if (NOT GOOSE_EXPLICIT_PLATFORM)

    add_executable(goose_platform_binary ${PLATFORM_CC})
    set_target_properties(goose_platform_binary PROPERTIES OUTPUT_NAME goose_platform_binary)
    set_target_properties(goose_platform_binary PROPERTIES RUNTIME_OUTPUT_DIRECTORY
            ${PROJECT_BINARY_DIR})
    add_custom_command(
            OUTPUT ${PROJECT_BINARY_DIR}/goose_platform_out
            DEPENDS goose_platform_binary
            COMMAND $<TARGET_FILE:goose_platform_binary> > ${PROJECT_BINARY_DIR}/goose_platform_out || (echo "Provide explicit GOOSE_PLATFORM=your_target_arch to avoid build-type detection of the platform" && exit 1)
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
else ()
    add_custom_command(
            OUTPUT ${PROJECT_BINARY_DIR}/goose_platform_out
            COMMAND
            ${CMAKE_COMMAND} -E echo_append \"${GOOSE_EXPLICIT_PLATFORM}\" > ${PROJECT_BINARY_DIR}/goose_platform_out
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endif ()

add_custom_target(goose_platform DEPENDS ${PROJECT_BINARY_DIR}/goose_platform_out)

function(goose_cc_extension)
    set(options
            PUBLIC
            EXCLUDE_SYSTEM
    )
    set(args NAME
            NAMESPACE
            EXTENSION_VERSION
            CAPI_VERSION
            ABI_TYPE
    )

    set(list_args
            DEPS
            SOURCES
            OBJECTS
            INCLUDES
            PINCLUDES
            DEFINES
            COPTS
            CXXOPTS
            CUOPTS
            LINKS
            PLINKS
            WLINKS
    )

    cmake_parse_arguments(
            PARSE_ARGV 0
            KMCMAKE_CC_LIB
            "${options}"
            "${args}"
            "${list_args}"
    )

    if ("${KMCMAKE_CC_LIB_NAME}" STREQUAL "")
        get_filename_component(KMCMAKE_CC_LIB_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
        string(REPLACE " " "_" KMCMAKE_CC_LIB_NAME ${KMCMAKE_CC_LIB_NAME})
        kmcmake_print(" Library, NAME argument not provided. Using folder name:  ${KMCMAKE_CC_LIB_NAME}")
    endif ()

    if ("${KMCMAKE_CC_LIB_NAMESPACE}" STREQUAL "")
        set(KMCMAKE_CC_LIB_NAMESPACE ${PROJECT_NAME})
        kmcmake_print(" Library, NAMESPACE argument not provided. Using target alias:  ${KMCMAKE_CC_LIB_NAME}::${KMCMAKE_CC_LIB_NAME}")
    endif ()

    if (NOT KMCMAKE_CC_LIB_ABI_TYPE)
        kmcmake_error("No ABI type given, should be one of [CPP|C_STRUCT|C_STRUCT_UNSTABLE]")
    endif ()

    if (NOT ${KMCMAKE_CC_LIB_ABI_TYPE} STREQUAL "CPP"
            AND NOT  ${KMCMAKE_CC_LIB_ABI_TYPE} STREQUAL "C_STRUCT"
            AND NOT  ${KMCMAKE_CC_LIB_ABI_TYPE} STREQUAL "C_STRUCT_UNSTABLE")
        kmcmake_error("Bad ABI type given, should be one of [CPP|C_STRUCT|C_STRUCT_UNSTABLE]")
    endif ()

    if (${KMCMAKE_CC_LIB_ABI_TYPE} STREQUAL "CPP")
        set(FOOTER_VERSION_VALUE ${GOOSE_NORMALIZED_VERSION})
    elseif (${KMCMAKE_CC_LIB_ABI_TYPE} STREQUAL "C_STRUCT_UNSTABLE")
        set(FOOTER_VERSION_VALUE ${GOOSE_NORMALIZED_VERSION})
    elseif (${KMCMAKE_CC_LIB_ABI_TYPE} STREQUAL "C_STRUCT")
        if(NOT KMCMAKE_CC_LIB_CAPI_VERSION)
            kmcmake_error("must assign CAPI_VERSION when C_STRUCT.")
        endif ()
        set(FOOTER_VERSION_VALUE ${KMCMAKE_CC_LIB_CAPI_VERSION})
    else ()
        kmcmake_error("Bad ABI type given, should be one of [CPP|C_STRUCT|C_STRUCT_UNSTABLE]")
    endif()

    kmcmake_raw("-----------------------------------")
    if (KMCMAKE_CC_LIB_PUBLIC)
        set(KMCMAKE_LIB_INFO "${KMCMAKE_CC_LIB_NAMESPACE}::${KMCMAKE_CC_LIB_NAME}  SHARED&STATIC PUBLIC")
    else ()
        set(KMCMAKE_LIB_INFO "${KMCMAKE_CC_LIB_NAMESPACE}::${KMCMAKE_CC_LIB_NAME}  SHARED&STATIC INTERNAL")
    endif ()

    set(${KMCMAKE_CC_LIB_NAME}_INCLUDE_SYSTEM SYSTEM)
    if (KMCMAKE_CC_LIB_EXCLUDE_SYSTEM)
        set(${KMCMAKE_CC_LIB_NAME}_INCLUDE_SYSTEM "")
    endif ()

    kmcmake_print_label("Create Library" "${KMCMAKE_LIB_INFO}")
    kmcmake_raw("-----------------------------------")
    if (VERBOSE_KMCMAKE_BUILD)
        kmcmake_print_list_label("Sources" KMCMAKE_CC_LIB_SOURCES)
        kmcmake_print_list_label("Objects" KMCMAKE_CC_LIB_OBJECTS)
        kmcmake_print_list_label("Deps" KMCMAKE_CC_LIB_DEPS)
        kmcmake_print_list_label("COPTS" KMCMAKE_CC_LIB_COPTS)
        kmcmake_print_list_label("CXXOPTS" KMCMAKE_CC_LIB_CXXOPTS)
        kmcmake_print_list_label("CUOPTS" KMCMAKE_CC_LIB_CUOPTS)
        kmcmake_print_list_label("Defines" KMCMAKE_CC_LIB_DEFINES)
        kmcmake_print_list_label("Includes" KMCMAKE_CC_LIB_INCLUDES)
        kmcmake_print_list_label("Private Includes" KMCMAKE_CC_LIB_PINCLUDES)
        kmcmake_print_list_label("Links" KMCMAKE_CC_LIB_LINKS)
        kmcmake_print_list_label("Private Links" KMCMAKE_CC_LIB_PLINKS)
        kmcmake_raw("-----------------------------------")
    endif ()
    set(KMCMAKE_CC_LIB_OBJECTS_FLATTEN)
    if (KMCMAKE_CC_LIB_OBJECTS)
        foreach (obj IN LISTS KMCMAKE_CC_LIB_OBJECTS)
            list(APPEND KMCMAKE_CC_LIB_OBJECTS_FLATTEN $<TARGET_OBJECTS:${obj}>)
        endforeach ()
    endif ()

        add_library(${KMCMAKE_CC_LIB_NAME}_OBJECT OBJECT ${KMCMAKE_CC_LIB_SOURCES} ${KMCMAKE_CC_LIB_HEADERS})
        list(APPEND KMCMAKE_CC_LIB_OBJECTS_FLATTEN $<TARGET_OBJECTS:${KMCMAKE_CC_LIB_NAME}_OBJECT>)
        if (KMCMAKE_CC_LIB_DEPS)
            add_dependencies(${KMCMAKE_CC_LIB_NAME}_OBJECT ${KMCMAKE_CC_LIB_DEPS})
        endif ()
        set_property(TARGET ${KMCMAKE_CC_LIB_NAME}_OBJECT PROPERTY POSITION_INDEPENDENT_CODE 1)
        target_compile_options(${KMCMAKE_CC_LIB_NAME}_OBJECT PRIVATE $<$<COMPILE_LANGUAGE:C>:${KMCMAKE_CC_LIB_COPTS}>)
        target_compile_options(${KMCMAKE_CC_LIB_NAME}_OBJECT PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${KMCMAKE_CC_LIB_CXXOPTS}>)
        target_compile_options(${KMCMAKE_CC_LIB_NAME}_OBJECT PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:${KMCMAKE_CC_LIB_CUOPTS}>)
        target_include_directories(${KMCMAKE_CC_LIB_NAME}_OBJECT ${${KMCMAKE_CC_LIB_NAME}_INCLUDE_SYSTEM}
                PUBLIC
                ${KMCMAKE_CC_LIB_INCLUDES}
                "$<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}>"
                "$<BUILD_INTERFACE:${${PROJECT_NAME}_BINARY_DIR}>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        )
        target_include_directories(${KMCMAKE_CC_LIB_NAME}_OBJECT ${${KMCMAKE_CC_LIB_NAME}_INCLUDE_SYSTEM}
                PRIVATE
                ${KMCMAKE_CC_LIB_PINCLUDES}
        )

        target_compile_definitions(${KMCMAKE_CC_LIB_NAME}_OBJECT
                PUBLIC
                ${KMCMAKE_CC_LIB_DEFINES}
        )

    list(LENGTH KMCMAKE_CC_LIB_OBJECTS_FLATTEN obj_len)
    if (obj_len EQUAL -1)
        kmcmake_error("no source or object give to the library ${KMCMAKE_CC_LIB_NAME}")
    endif ()
    add_library(${KMCMAKE_CC_LIB_NAME}_static STATIC ${KMCMAKE_CC_LIB_OBJECTS_FLATTEN})
    if (${KMCMAKE_CC_LIB_NAME}_OBJECT)
        add_dependencies(${KMCMAKE_CC_LIB_NAME}_static ${KMCMAKE_CC_LIB_NAME}_OBJECT)
    endif ()
    if (KMCMAKE_CC_LIB_DEPS)
        add_dependencies(${KMCMAKE_CC_LIB_NAME}_static ${KMCMAKE_CC_LIB_DEPS})
    endif ()
    target_link_libraries(${KMCMAKE_CC_LIB_NAME}_static PRIVATE ${KMCMAKE_CC_LIB_PLINKS})
    target_link_libraries(${KMCMAKE_CC_LIB_NAME}_static PUBLIC ${KMCMAKE_CC_LIB_LINKS})
    target_link_libraries(${KMCMAKE_CC_LIB_NAME}_static PRIVATE ${KMCMAKE_CC_LIB_WLINKS})
    set_target_properties(${KMCMAKE_CC_LIB_NAME}_static PROPERTIES OUTPUT_NAME goose_${KMCMAKE_CC_LIB_NAME})

    add_library(${KMCMAKE_CC_LIB_NAMESPACE}::goose_${KMCMAKE_CC_LIB_NAME} ALIAS ${KMCMAKE_CC_LIB_NAME}_static)
    target_include_directories(${KMCMAKE_CC_LIB_NAME}_static INTERFACE
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    )

    add_library(${KMCMAKE_CC_LIB_NAME}_shared SHARED ${KMCMAKE_CC_LIB_OBJECTS_FLATTEN})
    if (${KMCMAKE_CC_LIB_NAME}_OBJECT)
        add_dependencies(${KMCMAKE_CC_LIB_NAME}_shared ${KMCMAKE_CC_LIB_NAME}_OBJECT)
    endif ()
    if (KMCMAKE_CC_LIB_DEPS)
        add_dependencies(${KMCMAKE_CC_LIB_NAME}_shared ${KMCMAKE_CC_LIB_DEPS})
    endif ()
    target_link_libraries(${KMCMAKE_CC_LIB_NAME}_shared PRIVATE ${KMCMAKE_CC_LIB_PLINKS})
    target_link_libraries(${KMCMAKE_CC_LIB_NAME}_shared PUBLIC ${KMCMAKE_CC_LIB_LINKS})
    foreach (link ${KMCMAKE_CC_LIB_WLINKS})
        target_link_libraries(${KMCMAKE_CC_LIB_NAME}_shared PRIVATE $<LINK_LIBRARY:WHOLE_ARCHIVE,${link}>)
    endforeach ()
    target_compile_definitions(${KMCMAKE_CC_LIB_NAME}_shared PUBLIC -DGOOSE_BUILD_LOADABLE_EXTENSION)
    set_target_properties(${KMCMAKE_CC_LIB_NAME}_shared PROPERTIES DEFINE_SYMBOL "")
    set_target_properties(${KMCMAKE_CC_LIB_NAME}_shared PROPERTIES OUTPUT_NAME ${KMCMAKE_CC_LIB_NAME})
    set_target_properties(${KMCMAKE_CC_LIB_NAME}_shared PROPERTIES PREFIX "")
    set_target_properties(${KMCMAKE_CC_LIB_NAME}_shared PROPERTIES SUFFIX ".goose_extension")

    add_library(${KMCMAKE_CC_LIB_NAMESPACE}::goose_${KMCMAKE_CC_LIB_NAME}_loadable ALIAS ${KMCMAKE_CC_LIB_NAME}_shared)
    target_include_directories(${KMCMAKE_CC_LIB_NAME}_shared INTERFACE
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    )

    add_custom_command(
            TARGET ${KMCMAKE_CC_LIB_NAME}_shared
            POST_BUILD
            COMMAND
            ${CMAKE_COMMAND} -DABI_TYPE=${ABI_TYPE} -DEXTENSION=$<TARGET_FILE:${KMCMAKE_CC_LIB_NAME}_shared>${EXTENSION_POSTFIX} -DPLATFORM_FILE=${goose_BINARY_DIR}/goose_platform_out -DVERSION_FIELD="${FOOTER_VERSION_VALUE}" -DEXTENSION_VERSION="${KMCMAKE_CC_LIB_EXTENSION_VERSION}" -DNULL_FILE=${NULL_TXT} -P ${META_CMAKE}
    )
    add_dependencies(${KMCMAKE_CC_LIB_NAME}_shared goose_platform)
    if (KMCMAKE_CC_LIB_PUBLIC)
        install(TARGETS ${KMCMAKE_CC_LIB_NAME}_shared ${KMCMAKE_CC_LIB_NAME}_static
                EXPORT ${PROJECT_NAME}Targets
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        )
    endif ()

    foreach (arg IN LISTS KMCMAKE_CC_LIB_UNPARSED_ARGUMENTS)
        message(WARNING "Unparsed argument: ${arg}")
    endforeach ()

endfunction()