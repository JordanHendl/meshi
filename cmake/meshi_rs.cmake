set(MESHI_RS_VERSION 0.0.1)
set(MESHI_RS_BASE_URL https://github.com/JordanHendl/meshi-rs/releases/download/v${MESHI_RS_VERSION})

set(MESHI_RS_DIR ${CMAKE_BINARY_DIR}/meshi-rs)
set(RUST_TARGET_DIR ${MESHI_RS_DIR})

if(WIN32)
  set(MESHI_RS_ARTIFACT meshi.dll)
  set(MESHI_RS_LIBRARY_NAME meshi.dll)
else()
  set(MESHI_RS_ARTIFACT meshi.so)
  set(MESHI_RS_LIBRARY_NAME libmeshi.so)
endif()

set(MESHI_RS_URL ${MESHI_RS_BASE_URL}/${MESHI_RS_ARTIFACT})
set(MESHI_RS_DOWNLOAD ${MESHI_RS_DIR}/${MESHI_RS_ARTIFACT})
set(MESHI_RS_LIBRARY_PATH ${RUST_TARGET_DIR}/${MESHI_RS_LIBRARY_NAME})

add_custom_command(
  OUTPUT ${MESHI_RS_DOWNLOAD}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${MESHI_RS_DIR}
  COMMAND ${CMAKE_COMMAND} -DURL=${MESHI_RS_URL} -DFILE=${MESHI_RS_DOWNLOAD}
          -P ${CMAKE_CURRENT_LIST_DIR}/download_meshi_rs.cmake
  COMMENT "Downloading meshi-rs from ${MESHI_RS_URL}"
)

add_custom_target(download_meshi_rs DEPENDS ${MESHI_RS_DOWNLOAD})

add_custom_command(
  OUTPUT ${MESHI_RS_LIBRARY_PATH}
  DEPENDS ${MESHI_RS_DOWNLOAD}
  COMMAND ${CMAKE_COMMAND} -E copy ${MESHI_RS_DOWNLOAD} ${MESHI_RS_LIBRARY_PATH}
  COMMENT "Copying meshi-rs library to ${MESHI_RS_LIBRARY_PATH}"
)

add_custom_target(copy_meshi_library DEPENDS ${MESHI_RS_LIBRARY_PATH})

add_library(meshi-rs SHARED IMPORTED)

if(WIN32)
  set_target_properties(meshi-rs PROPERTIES
    IMPORTED_LOCATION ${MESHI_RS_LIBRARY_PATH}
    IMPORTED_IMPLIB ${RUST_TARGET_DIR}/meshi.dll.lib
  )
else()
  set_target_properties(meshi-rs PROPERTIES
    IMPORTED_LOCATION ${MESHI_RS_LIBRARY_PATH}
  )
endif()

add_dependencies(meshi-rs copy_meshi_library)
