# Create the interface target
add_library(meshi INTERFACE)

# Specify the include directories for the target
target_include_directories(meshi INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # During build
    $<INSTALL_INTERFACE:include>                           # During install
)

# Optional: Specify C++ standard for the interface target
target_compile_features(meshi INTERFACE cxx_std_17) # Adjust standard as needed

# Optional: Add any compiler flags or options
#target_compile_options(meshi INTERFACE /std:c++latest)

# Optional: Add any dependencies for the interface target
target_link_libraries(meshi INTERFACE meshi-rs glm)

# Optional: If you want to install the library
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
)
