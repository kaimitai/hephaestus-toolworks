cmake_minimum_required(VERSION 3.20)
project(HephaestusToolworks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 REQUIRED)

# Core (library)
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    src/core/*.cpp
)

add_library(hephaestus.core ${CORE_SOURCES})

target_include_directories(hephaestus.core PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# Forge (CLI)
file(GLOB_RECURSE FORGE_SOURCES CONFIGURE_DEPENDS
    src/forge/*.cpp
)

add_executable(forge ${FORGE_SOURCES})

target_link_libraries(forge PRIVATE hephaestus.core)

# Workbench (SDL)
file(GLOB_RECURSE WORKBENCH_SOURCES CONFIGURE_DEPENDS
    src/workbench/*.cpp
)

add_executable(workbench ${WORKBENCH_SOURCES})

target_link_libraries(workbench
    PRIVATE
        hephaestus.core
        SDL3::SDL3
)
