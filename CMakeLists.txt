cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(APG)

find_package(OpenGL)
find_package(GLUT)
find_package(SDL2)
find_package(SDL2_image)
find_package(tmxparser)


set(CMAKE_CXX_FLAGS "-std=c++1y -g3 -O0 -Wall -Wextra")
add_subdirectory("src")

include_directories("include")

add_executable(glRenderTest
