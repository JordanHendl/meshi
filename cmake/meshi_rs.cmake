set(MESHI_RS_PROJECT_DIR ${CMAKE_SOURCE_DIR}/meshi-rs)

add_custom_target(cargo_build ALL
    COMMAND cargo build
    WORKING_DIRECTORY ${MESHI_RS_PROJECT_DIR}
    COMMENT "Building Rust project with cargo"
    USES_TERMINAL
)

add_custom_target(copy_meshi_library
    COMMENT "This target depends on cargo_build"
    DEPENDS cargo_build meshi-rs-cpy
)


add_library(meshi-rs SHARED IMPORTED)

if(WIN32)
  set(MESHI_RS_LIBRARY ${MESHI_RS_PROJECT_DIR}/target/debug/meshi.dll)
  set(MESHI_RS_IMPLIB ${MESHI_RS_PROJECT_DIR}/target/debug/meshi.dll.lib)

  set_target_properties(meshi-rs PROPERTIES
    IMPORTED_LOCATION ${MESHI_RS_LIBRARY}
    IMPORTED_IMPLIB ${MESHI_RS_IMPLIB}
  )
elseif(UNIX)
  set(MESHI_RS_LIBRARY ${MESHI_RS_PROJECT_DIR}/target/debug/libmeshi.so)
  set_target_properties(meshi-rs PROPERTIES
    IMPORTED_LOCATION ${MESHI_RS_LIBRARY}
  )
endif()

add_custom_command(
  OUTPUT meshi-rs-cpy
  COMMAND ${CMAKE_COMMAND} -E copy ${MESHI_RS_LIBRARY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


add_dependencies(meshi-rs copy_meshi_library)

