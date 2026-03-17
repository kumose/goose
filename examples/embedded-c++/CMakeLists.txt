cmake_minimum_required(VERSION 2.8.12...3.29)
project(example-c++)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/src/include)
link_directories(${PROJECT_SOURCE_DIR}/build/release/src)

add_executable(example main.cc)
target_link_libraries(example goose)
