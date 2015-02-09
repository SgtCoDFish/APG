cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN32 0)

project(APG)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message("Conifguring APG version ${VERSION}.")

file(GLOB_RECURSE APG_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE APG_HEADERS ${PROJECT_SOURCE_DIR}/include/*.hpp)

if(NOT DEFINED TMXPARSER_INCLUDE_DIR)
    message("No tmxparser include dir specified")
else()
    include_directories($TMXPARSER_INCLUDE_DIR)
endif()

if(NOT DEFINED TMXPARSER_LIBRARIES)
    message("No tmxparser library specified")
else()
    link_directories($TMXPARSER_LIBRARIES)
endif()


include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image)

find_package(OpenGL)
find_package(GLUT)

set(CMAKE_CXX_COMPILER_ARG1 "-std=c++1y")
set(CMAKE_CXX_FLAGS_BASE "-Wall -Wextra -Wno-unused-parameter -fPIC")

set(APG_DEBUG_FLAGS "-g3 -O0")
set(APG_RELEASE_FLAGS "-g0 -O3")

include_directories("include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_DEBUG_FLAGS}")
add_library(APG-d ${APG_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_RELEASE_FLAGS}")
add_library(APG ${APG_SOURCES})

set (SDLRENDERTEST_SOURCES
    test/APGSDLRenderTest.cpp)

set (GLRENDERTEST_SOURCES
    test/APGGLRenderTest.cpp)

set (GLRENDERTESTTEST_FLAGS "")
set (SDLRENDERTEST_FLAGS "-DAPG_TEST_SDL")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_DEBUG_FLAGS} ${SDLRENDERTEST_FLAGS}")
add_executable(SDLRenderTest EXCLUDE_FROM_ALL ${SDLRENDERTEST_SOURCES})
target_link_libraries(SDLRenderTest APG-d SDL2 SDL2_image GL GLU GLEW tmxparser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_DEBUG_FLAGS} ${GLRENDERTEST_FLAGS}")
add_executable(GLRenderTest EXCLUDE_FROM_ALL ${GLRENDERTEST_SOURCES})
target_link_libraries(GLRenderTest APG-d SDL2 SDL2_image GL GLU GLEW tmxparser)
