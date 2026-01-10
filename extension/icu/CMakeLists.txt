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
  find_package(
    ICU
    COMPONENTS i18n uc data
    REQUIRED)
  target_link_libraries(icu_extension ICU::i18n ICU::uc ICU::data)
disable_target_warnings(icu_extension)
set(PARAMETERS "-no-warnings")
build_loadable_extension(icu ${PARAMETERS} ${ICU_EXTENSION_FILES})

target_link_libraries(icu_loadable_extension ICU::i18n ICU::uc ICU::data)

install(
  TARGETS icu_extension
  EXPORT "${GOOSE_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")
