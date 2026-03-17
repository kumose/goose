cmake_minimum_required(VERSION 2.8.12...3.29)

project(ICUExtension)

include_directories(include)


set(ICU_EXTENSION_FILES
        ${ICU_LIBRARY_FILES}
        icu_extension.cc
        icu-current.cc
        icu-dateadd.cc
        icu-datefunc.cc
        icu-datepart.cc
        icu-datesub.cc
        icu-datetrunc.cc
        icu-makedate.cc
        icu-list-range.cc
        icu-table-range.cc
        icu-strptime.cc
        icu-timebucket.cc
        icu-timezone.cc)

build_static_extension(icu ${ICU_EXTENSION_FILES})
link_threads(icu_extension "")
find_package(xicu REQUIRED)
target_link_libraries(icu_extension xicu::xicu_static)
disable_target_warnings(icu_extension)
set(PARAMETERS "-no-warnings")
build_loadable_extension(icu ${PARAMETERS} ${ICU_EXTENSION_FILES})

target_link_libraries(icu_loadable_extension xicu::xicu_static)

install(
        TARGETS icu_extension
        EXPORT "${GOOSE_EXPORT_SET}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
