cmake_minimum_required(VERSION 3.13)
project(meshi-cpp)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

FILE(COPY ${CMAKE_SOURCE_DIR}/defaults/db.json DESTINATION ${CMAKE_BINARY_DIR}/database/)
include(meshi_rs)

include(FetchContent)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 7a878c2
)

FetchContent_MakeAvailable(glm)

set(SAMPLE_APP_DIR ${CMAKE_SOURCE_DIR}/sample_app_dir)
add_subdirectory(src)
