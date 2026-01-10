add_library_unity(
  goose_core_functions_date
  OBJECT
  current.cc
  age.cc
  date_diff.cc
  date_sub.cc
  to_interval.cc
  time_bucket.cc
  date_trunc.cc
  epoch.cc
  date_part.cc
  make_date.cc)
set(CORE_FUNCTION_FILES
    ${CORE_FUNCTION_FILES} $<TARGET_OBJECTS:goose_core_functions_date>
    PARENT_SCOPE)

# https://learn.microsoft.com/en-us/cpp/build/reference/ob-inline-function-expansion?view=msvc-170
# Lower functions inlining, /Ob2 causes ICE with ARM64 toolchain (both
# cross-compile and native): error MSB6006: "CL.exe" exited with code -529706956
if(MSVC
   AND (CMAKE_VS_PLATFORM_NAME STREQUAL "ARM64")
   AND (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL
                                               "RelWithDebInfo"))
  set_source_files_properties(ub_goose_core_functions_date
                              PROPERTIES COMPILE_OPTIONS /Ob1)
endif()
