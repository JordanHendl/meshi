set(MESHI_RS_PROJECT_DIR ${CMAKE_SOURCE_DIR}/meshi-rs)
set(CARGO_STAMP_FILE ${CMAKE_CURRENT_BINARY_DIR}/cargo_build_stamp)

#TODO add support for pulling rust headers and targets from 
# https://github.com/JordanHendl/meshi-rs/releases/download/v0.0.1/meshi.so
# https://github.com/JordanHendl/meshi-rs/releases/download/v0.0.1/meshi.dll

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

