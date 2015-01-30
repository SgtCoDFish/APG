cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN32 0)

project(APG)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message("Conifguring APG version ${VERSION}.")

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/*.hpp)

find_package(OpenGL)
find_package(GLUT)
find_package(SDL2)
find_package(SDL2_image)
find_package(tmxparser)

set(CMAKE_CXX_COMPILER_ARG1 "-std=c++1y")
set(CMAKE_CXX_FLAGS_BASE "-Wall -Wextra -fPIC")

set(APG_DEBUG_FLAGS "-g3 -O0")
set(APG_RELEASE_FLAGS "-g0 -O3")

include_directories("include")

add_subdirectory("src")
add_subdirectory("test")

