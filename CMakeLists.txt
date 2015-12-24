cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN32 0)

project(APG)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)
set(VERSION_PATCH 5)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message("Conifguring APG version ${VERSION}.")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if ( CMAKE_BUILD_TYPE MATCHES Release )
    message("Building release library.")
    set (APG_DEBUG FALSE)
else ()
    message("Building debug library.")
    set (APG_DEBUG TRUE)
endif ()

option(APG_NO_SDL "Do not build any component which uses SDL; removes SDL as a dependency. At present this leaves most of APG fairly unusable. Also currently sets APG_NO_GL." OFF)
option(APG_NO_GL "Do not build any component which uses OpenGL/GLEW, removing those as dependencies. Affects graphics rendering capabilities. Also currently sets APG_NO_SDL." OFF)
option(APG_NO_NATIVE "Do not build any component which uses functionality which differs across platforms (think #ifdef _WIN32). Useful if you're trying to build on some arcane platform." OFF)

option(EXCLUDE_GL_TEST "Should we exclude compiling the OpenGL TMX rendering test?" OFF)
option(EXCLUDE_SDL_TEST "Should we exclude compiling the SDL TMX rendering test?" OFF)
option(EXCLUDE_AUDIO_TEST "Should we exclude compiling the audio test? Requires an additional file, test_music.ogg, to be added to the assets folder (and then re-running cmake)!" ON)

if( APG_NO_GL OR APG_NO_SDL )
    message("Disabling GL, SDL and all tests because either APG_NO_GL or APG_NO_SDL was specified.")
    set(APG_NO_SDL ON)
    set(APG_NO_GL ON)
    
    set(EXCLUDE_GL_TEST ON)
    set(EXCLUDE_SDL_TEST ON)
    set(EXCLUDE_AUDIO_TEST ON)
endif ()

if( APG_NO_NATIVE )
    message("Warning: APG_NO_NATIVE is not thoroughly tested so there could be bugs!")
endif()

message ("Finding dependencies for APG.") 

if ( NOT APG_NO_SDL )
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_net REQUIRED)
endif ()

if ( NOT APG_NO_GL )
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
endif ()

find_package(GLM REQUIRED)
find_package(rapidjson REQUIRED)

find_package(TmxParser REQUIRED)
find_package(TinyXML2 REQUIRED)
find_package(ZLIB REQUIRED)

file(GLOB_RECURSE APG_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE APG_HEADERS ${PROJECT_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE APG_TEST_ASSETS ${PROJECT_SOURCE_DIR}/assets/*)

if( DEFINED WIN32 )
    if( MSVC )
        set (OS_FLAGS "")
        set (COMPILER_FLAGS "/GR /EHsc")
        set (OS_LIBS "")

        # VS 2015 has a linker issue when using the official SDL2 libs, this should fix it
        if( MSVC_VERSION GREATER 1800)
            message ("Using legacy STDIO fix for MSVC 15.")
            set(OS_LIBS ${OS_LIBS} legacy_stdio_definitions)
            set(EXTRA_SOURCES ${PROJECT_SOURCE_DIR}/MSVC/vs2015_io_fix.cpp)
        endif()
    else ()
        # MinGW
        set (OS_FLAGS "-mwindows")
        set (OS_LIBS Version Imm32 winmm vorbisfile ogg vorbis winsock32 mingw32)
    endif ()
else ()
    set (OS_FLAGS "-fPIC")
    set (OS_LIBS "")
endif ()

if ( NOT MSVC )
    set(CMAKE_CXX_COMPILER_ARG1 "-std=c++1y")
    set(COMPILER_FLAGS "-Wall -Wextra -Wno-unused-parameter ${OS_FLAGS}")
    set(APG_DEBUG_FLAGS "-g3 -O0")
    set(APG_RELEASE_FLAGS "-g0 -O3")
else () 
    set(APG_DEBUG_FLAGS "/Od /Gs")
    set(APG_RELEASE_FLAGS "/Ox")
endif ()

set(APG_SOURCES ${APG_SOURCES} ${EXTRA_SOURCES})

add_definitions(-DGLM_FORCE_RADIANS)

if ( APG_NO_SDL )
    add_definitions(-DAPG_NO_SDL)
endif ()

if ( APG_NO_GL )
    add_definitions(-DAPG_NO_GL)
endif()

if ( APG_NO_NATIVE)
    add_definitions(-DAPG_NO_NATIVE)
endif()

set(CMAKE_CXX_FLAGS_BASE "${CMAKE_CXX_FLAGS_BASE} ${COMPILER_FLAGS}")
message(${CMAKE_CXX_FLAGS_BASE})

include_directories("include"
                    ${GLM_INCLUDE_DIRS}
                    ${TMXPARSER_INCLUDE_DIR}
                    ${TINYXML2_INCLUDE_DIR}
                    ${ZLIB_INCLUDE_DIRS}
                    )

if ( NOT APG_NO_SDL )
    include_directories(${SDL2_INCLUDE_DIR}
                        ${SDL2_TTF_INCLUDE_DIR}
                        ${SDL2_IMAGE_INCLUDE_DIR}
                        ${SDL2_MIXER_INCLUDE_DIR}
                        ${SDL2_NET_INCLUDE_DIR}
                        )
endif ()

if ( NOT APG_NO_GL )
    include_directories(${OPENGL_INCLUDE_DIR}
                        ${GLEW_INCLUDE_DIRS}
                        )
endif ()

set(APG_LIB_NAME ${CMAKE_PROJECT_NAME})

if ( APG_DEBUG ) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_DEBUG_FLAGS}")
    set(APG_LIB_NAME "${APG_LIB_NAME}-d")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_RELEASE_FLAGS}")
endif ()

add_library(${APG_LIB_NAME} ${APG_SOURCES})

if ( NOT EXCLUDE_AUDIO_TEST )
    if ( NOT EXISTS ${PROJECT_SOURCE_DIR}/assets/test_music.ogg )
        message("You haven't added a \"test_music.ogg\" file to the assets folder. Without this, the audio test will fail to run; don't forget to add it!")
    endif ( )
endif ( )

if ( (NOT EXCLUDE_SDL_TEST) OR (NOT EXCLUDE_GL_TEST) OR (NOT EXCLUDE_AUDIO_TEST) ) 
	file(MAKE_DIRECTORY assets)
	file(COPY ${APG_TEST_ASSETS} DESTINATION assets)
	set(EXAMPLE_LIBS ${SDL2_LIBRARY}
					 ${SDL2_TTF_LIBRARIES}
					 ${SDL2_IMAGE_LIBRARY}
					 ${SDL2_MIXER_LIBRARY}
					 ${SDL2_NET_LIBRARY}
					 ${GLEW_LIBRARIES}
					 ${OPENGL_LIBRARIES}
					 ${TMXPARSER_LIBRARY}
					 ${TINYXML2_LIBRARIES}
					 ${ZLIB_LIBRARIES}
					 ${OS_LIBS})	
endif ( )

if ( NOT EXCLUDE_SDL_TEST)
	set (SDLRENDERTEST_FLAGS "")
	set (SDLRENDERTEST_SOURCES test/APGSDLRenderTest.cpp)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SDLRENDERTEST_FLAGS}")
	
	add_executable(SDLRenderTest ${SDLRENDERTEST_SOURCES} ${EXTRA_SOURCES})
	target_link_libraries(SDLRenderTest ${APG_LIB_NAME} ${EXAMPLE_LIBS})
endif ( NOT EXCLUDE_SDL_TEST )

if ( NOT EXCLUDE_GL_TEST )
	set (GLRENDERTEST_FLAGS "")
	set (GLRENDERTEST_SOURCES test/APGGLRenderTest.cpp)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GLRENDERTEST_FLAGS}")
	
	add_executable(GLRenderTest ${GLRENDERTEST_SOURCES} ${EXTRA_SOURCES})
	target_link_libraries(GLRenderTest ${APG_LIB_NAME} ${EXAMPLE_LIBS})
endif ( NOT EXCLUDE_GL_TEST )

if ( NOT EXCLUDE_AUDIO_TEST )
    set (AUDIOTEST_FLAGS "")
    set (AUDIOTEST_SOURCES test/APGAudioTest.cpp)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AUDIOTEST_FLAGS}")

    add_executable(AudioTest ${AUDIOTEST_SOURCES} ${EXTRA_SOURCES})
    target_link_libraries(AudioTest ${APG_LIB_NAME} ${EXAMPLE_LIBS})
endif ( NOT EXCLUDE_AUDIO_TEST )

install (TARGETS ${APG_LIB_NAME} DESTINATION lib)
install (DIRECTORY ${PROJECT_SOURCE_DIR}/include/APG DESTINATION include)
