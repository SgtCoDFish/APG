cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN32 0)

project(APG)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)
set(VERSION_PATCH 0)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message("Conifguring APG version ${VERSION}.")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

find_package(TmxParser REQUIRED)

file(GLOB_RECURSE APG_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE APG_HEADERS ${PROJECT_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE APG_TEST_ASSETS ${PROJECT_SOURCE_DIR}/assets/*)

if( DEFINED WIN32 )
	set (OS_FLAGS "-mwindows")
	set (OS_LIBS Version Imm32 winmm vorbisfile ogg vorbis mingw32)
else ()
	set (OS_FLAGS "-fPIC")
	set (OS_LIBS "")
endif ()

option(EXCLUDE_GL_TEST "Should we exclude compiling the OpenGL TMX rendering test?" OFF)
option(EXCLUDE_SDL_TEST "Should we exclude compiling the SDL TMX rendering test?" OFF)
option(EXCLUDE_AUDIO_TEST "Should we exclude compiling the audio test? Required an additional file, test_music.ogg, to be added to the assets folder (and then re-running cmake)!" ON)

set(CMAKE_CXX_COMPILER_ARG1 "-std=c++1y")
set(CMAKE_CXX_FLAGS_BASE "-Wall -Wextra -Wno-unused-parameter ${OS_FLAGS}")

set(APG_DEBUG_FLAGS "-g3 -O0")
set(APG_RELEASE_FLAGS "-g0 -O3")

include_directories("include" ${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${TMXPARSER_INCLUDE_DIR} ${GLM_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_DEBUG_FLAGS}")
add_library(APG-d ${APG_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_RELEASE_FLAGS}")
add_library(APG ${APG_SOURCES})

if ( NOT EXISTS ${PROJECT_SOURCE_DIR}/assets/test_music.ogg )
    if ( NOT EXCLUDE_AUDIO_TEST )
        message("You haven't added a \"test_music.ogg\" file to the assets folder. Without this, the audio test will fail to run; don't forget to add it!")
    endif ( NOT EXCLUDE_AUDIO_TEST )
endif ( NOT EXISTS ${PROJECT_SOURCE_DIR}/assets/test_music.ogg )

if ( (NOT EXCLUDE_SDL_TEST) OR (NOT EXCLUDE_GL_TEST) OR (NOT EXCLUDE_AUDIO_TEST) ) 
	file(MAKE_DIRECTORY assets)
	file(COPY ${APG_TEST_ASSETS} DESTINATION assets)
	set(EXAMPLE_LIBS ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARY} ${SDL2_MIXER_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${TMXPARSER_LIBRARY} ${OS_LIBS})	
endif ( (NOT EXCLUDE_SDL_TEST) OR (NOT EXCLUDE_GL_TEST) OR (NOT EXCLUDE_AUDIO_TEST) )

if ( NOT EXCLUDE_SDL_TEST)
	set (SDLRENDERTEST_FLAGS "")
	set (SDLRENDERTEST_SOURCES test/APGSDLRenderTest.cpp)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_DEBUG_FLAGS} ${SDLRENDERTEST_FLAGS}")
	
	add_executable(SDLRenderTest ${SDLRENDERTEST_SOURCES})
	target_link_libraries(SDLRenderTest APG-d ${EXAMPLE_LIBS})
endif ( NOT EXCLUDE_SDL_TEST )

if ( NOT EXCLUDE_GL_TEST )
	set (GLRENDERTEST_FLAGS "")
	set (GLRENDERTEST_SOURCES test/APGGLRenderTest.cpp)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_DEBUG_FLAGS} ${GLRENDERTEST_FLAGS}")
	
	add_executable(GLRenderTest ${GLRENDERTEST_SOURCES})
	target_link_libraries(GLRenderTest APG-d ${EXAMPLE_LIBS})
endif ( NOT EXCLUDE_GL_TEST )

if ( NOT EXCLUDE_AUDIO_TEST )
    set (AUDIOTEST_FLAGS "")
    set (AUDIOTEST_SOURCES test/APGAudioTest.cpp)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_DEBUG_FLAGS} ${AUDIOTEST_FLAGS}")

    add_executable(AudioTest ${AUDIOTEST_SOURCES})
    target_link_libraries(AudioTest APG-d ${EXAMPLE_LIBS})
endif ( NOT EXCLUDE_AUDIO_TEST )

install (TARGETS APG-d DESTINATION lib)
install (TARGETS APG DESTINATION lib)
install (DIRECTORY ${PROJECT_SOURCE_DIR}/include/APG DESTINATION include)
