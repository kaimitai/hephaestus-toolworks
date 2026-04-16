cmake_minimum_required(VERSION 3.20)
project(HephaestusToolworks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_executable(hephaestus.forge ${FORGE_SOURCES})

target_link_libraries(hephaestus.forge PRIVATE hephaestus.core)
