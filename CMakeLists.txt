cmake_minimum_required(VERSION 3.21)
project(SatelliteOrbitSimulator LANGUAGES CXX)

find_package(SDL3 3.2 CONFIG REQUIRED)

# Engine.cpp provides main(); the other sources implement physics and meshes.
add_executable(SatelliteOrbit
    Engine.cpp
    functions.cpp
    shapes.cpp
    satellite.h
    mesh.h
    shapes.h
    stars.h
)
target_compile_features(SatelliteOrbit PRIVATE cxx_std_17)
set_target_properties(SatelliteOrbit PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(SatelliteOrbit PRIVATE SDL3::SDL3)

# Keep console input available: do not use the WIN32 executable option.
# A shared SDL installation needs its DLL beside the executable on Windows.
if(WIN32 AND TARGET SDL3::SDL3-shared)
    add_custom_command(TARGET SatelliteOrbit POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "$<TARGET_FILE:SDL3::SDL3-shared>"
            "$<TARGET_FILE_DIR:SatelliteOrbit>"
        VERBATIM
    )
endif()

# Build first, then launch with access to the terminal for satellite input.
add_custom_target(run
    COMMAND "$<TARGET_FILE:SatelliteOrbit>"
    DEPENDS SatelliteOrbit
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:SatelliteOrbit>"
    USES_TERMINAL
    VERBATIM
)
