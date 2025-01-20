set(MESHI_RS_PROJECT_DIR ${CMAKE_SOURCE_DIR}/meshi-rs)

add_custom_target(cargo_build ALL
    COMMAND cargo build
    WORKING_DIRECTORY ${MESHI_RS_PROJECT_DIR}
    COMMENT "Building Rust project with cargo"
    USES_TERMINAL
)

# Add a dependency target to wait on `cargo_build`
add_custom_target(dependency_target
    COMMENT "This target depends on cargo_build"
    DEPENDS cargo_build
)

add_library(meshi-rs SHARED IMPORTED)
add_dependencies(meshi-rs cargo_build)


if(WIN32)
  set(MESHI_RS_LIBRARY ${MESHI_RS_PROJECT_DIR}/target/debug/meshi.dll)
  set(MESHI_RS_IMPLIB ${MESHI_RS_PROJECT_DIR}/target/debug/meshi.dll.lib)

  set_target_properties(meshi-rs PROPERTIES
    IMPORTED_LOCATION ${MESHI_RS_LIBRARY}
    IMPORTED_IMPLIB ${MESHI_RS_IMPLIB}
  )
  
  file(COPY ${MESHI_RS_LIBRARY} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
elseif(UNIX)
  set(MESHI_RS_LIBRARY ${MESHI_RS_PROJECT_DIR}/target/debug/meshi.so)
  set_target_properties(meshi-rs PROPERTIES
    IMPORTED_LOCATION ${MESHI_RS_LIBRARY}
  )

  file(COPY ${MESHI_RS_LIBRARY} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
