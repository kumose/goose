include_directories(include)


add_library(dbgen OBJECT
  dbgen.cc
  dbgen_gunk.cc
  bm_utils.cc
  build.cc
  permute.cc
  rnd.cc
  rng64.cc
  speed_seed.cc
  text.cc)
set(DBGEN_OBJECT_FILES
   ${DBGEN_OBJECT_FILES} $<TARGET_OBJECTS:dbgen>
   PARENT_SCOPE)

disable_target_warnings(dbgen)
