set(MESHI_RS_PROJECT_DIR ${CMAKE_SOURCE_DIR}/meshi-rs)
set(CARGO_STAMP_FILE ${CMAKE_CURRENT_BINARY_DIR}/cargo_build_stamp)

# Configure cargo build command based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CARGO_CMD cargo build --release)
  set(RUST_TARGET_DIR ${MESHI_RS_PROJECT_DIR}/target/release)
else()
  set(CARGO_CMD cargo build)
  set(RUST_TARGET_DIR ${MESHI_RS_PROJECT_DIR}/target/debug)
endif()

## Path to your Rust output
if(WIN32)
  set(RUST_OUTPUT ${MESHI_RS_PROJECT_DIR}/target/debug/meshi.dll)
else()
  set(RUST_OUTPUT ${MESHI_RS_PROJECT_DIR}/target/debug/libmeshi.so)
endif()

# Custom command to build the Rust lib directly
add_custom_command(
    OUTPUT ${RUST_OUTPUT}
    COMMAND cargo build
    WORKING_DIRECTORY ${MESHI_RS_PROJECT_DIR}
    COMMENT "Building Rust project with cargo"
    VERBATIM
)

# Target that builds the Rust lib
add_custom_target(cargo_build ALL
    DEPENDS ${RUST_OUTPUT}
)

# Target to copy it
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/meshi_copied_marker
    COMMAND ${CMAKE_COMMAND} -E copy ${RUST_OUTPUT} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/meshi_copied_marker
    DEPENDS ${RUST_OUTPUT}
    COMMENT "Copying Rust library"
)

add_custom_target(copy_meshi_library
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/meshi_copied_marker
)
#Declare the imported Rust library for use in linking
add_library(meshi-rs SHARED IMPORTED)

if(WIN32)
  set_target_properties(meshi-rs PROPERTIES
    IMPORTED_LOCATION ${RUST_TARGET_DIR}/meshi.dll
    IMPORTED_IMPLIB ${RUST_TARGET_DIR}/meshi.dll.lib
  )
else()
  set_target_properties(meshi-rs PROPERTIES
    IMPORTED_LOCATION ${RUST_TARGET_DIR}/libmeshi.so
  )
endif()

# Ensure the copy runs before anything that uses meshi-rs
add_dependencies(meshi-rs copy_meshi_library)

