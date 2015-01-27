cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
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

set(LIB_FLAGS "-std=c++1y -O3 -g0 -Wall -Wextra")
set(SDLTEST_FLAGS "-std=c++1y -g3 -O0 -Wall -Wextra -DAPG_TEST_SDL")
set(GLTEST_FLAGS "-std=c++1y -g3 -O0 -Wall -Wextra")

add_subdirectory("src")

add_library(APG SHARED ${SOURCES} ${HEADERS})
set_target_properties(APG PROPERTIES
    COMPILE_FLAGS ${LIB_FLAGS})

add_executable(APG_glRenderTest ${SOURCES} ${HEADERS} test/APGGLRenderTest.cpp)
add_executable(APG_sdlRenderTest ${SOURCES} ${HEADERS} test/APGSDLRenderTest.cpp)

