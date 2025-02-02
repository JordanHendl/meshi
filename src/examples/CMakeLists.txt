add_definitions(-DEXAMPLE_APP_DIR="${SAMPLE_APP_DIR}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(hello_engine)
add_subdirectory(hello_lighting)
