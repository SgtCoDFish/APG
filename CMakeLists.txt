cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
set(CMAKE_LEGACY_CYGWIN32 0)

project(APG)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)
set(VERSION_PATCH 0)

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

option(EXCLUDE_GL_TEST "Should we exclude compiling the OpenGL TMX rendering test?" OFF)
option(EXCLUDE_SDL_TEST "Should we exclude compiling the SDL TMX rendering test?" OFF)
option(EXCLUDE_AUDIO_TEST "Should we exclude compiling the audio test? Requires an additional file, test_music.ogg, to be added to the assets folder (and then re-running cmake)!" ON)

option(EXCLUDE_APGMAIN "Should we exclude building the main APG library, which has many dependencies?" OFF)
option(EXCLUDE_APGTILED "Should we exclude building the APG tiled library?" OFF)
option(EXCLUDE_APGNET "Should we exclude building the APGnet library, which depends on SDL2_net?" OFF)

if ( EXCLUDE_APGMAIN AND EXCLUDE_APGTILED AND EXCLUDE_APGNET )
    message(FATAL_ERROR "All libraries excluded, nothing to do!")
endif ()

if ( EXCLUDE_APGMAIN OR EXCLUDE_APGTILED OR EXCLUDE_APGNET )
    message("Warning: library exclusion not yet implemented.")
endif ()

message ("Finding dependencies for the main APG library.") 

find_package(GLM REQUIRED)
find_package(rapidjson REQUIRED)

if ( NOT EXCLUDE_APGMAIN OR NOT EXCLUDE_APGTILED )
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
endif ()

if ( NOT EXCLUDE_APGTILED )
    message ("Finding dependencies for APGtiled")
    find_package(TmxParser REQUIRED)
    find_package(TinyXML2 REQUIRED)
    find_package(ZLIB REQUIRED)
endif ()

if ( NOT EXCLUDE_APGNET )
    message ("Finding dependencies for APGnet")
    find_package(SDL2_net REQUIRED)
endif ()

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
        set (OS_LIBS Version Imm32 winmm vorbisfile ogg vorbis mingw32)
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

set(CMAKE_CXX_FLAGS_BASE ${COMPILER_FLAGS})

include_directories("include"
                    ${SDL2_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR}
                    ${SDL2_IMAGE_INCLUDE_DIR}
                    ${SDL2_MIXER_INCLUDE_DIR}
                    ${OPENGL_INCLUDE_DIR}
                    ${GLEW_INCLUDE_DIRS}
                    ${GLM_INCLUDE_DIRS}
                    )

if ( NOT EXCLUDE_APGTILED )
    include_directories(${TMXPARSER_INCLUDE_DIR}
                        ${TINYXML2_INCLUDE_DIR}
                        ${ZLIB_INCLUDE_DIRS}
                        )
endif ()

if ( NOT EXCLUDE_APGNET )
    include_directories(${SDL2_NET_INCLUDE_DIR}
                        )
endif ()

set(APG_LIB_NAME ${CMAKE_PROJECT_NAME})
set(APGTILED_LIB_NAME "${CMAKE_PROJECT_NAME}tiled")
set(APGNET_LIB_NAME "${CMAKE_PROJECT_NAME}net")

if ( APG_DEBUG ) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_DEBUG_FLAGS}")
    set(APG_LIB_NAME "${APG_LIB_NAME}-d")
    set(APGTILED_LIB_NAME "${APGTILED_LIB_NAME}-d")
    set(APGNET_LIB_NAME "${APGNET_LIB_NAME}-d")
else ()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BASE} ${APG_RELEASE_FLAGS}")
endif ()

add_library(${APG_LIB_NAME} ${APG_SOURCES})

if ( NOT EXISTS ${PROJECT_SOURCE_DIR}/assets/test_music.ogg )
    if ( NOT EXCLUDE_AUDIO_TEST )
        message("You haven't added a \"test_music.ogg\" file to the assets folder. Without this, the audio test will fail to run; don't forget to add it!")
    endif ( NOT EXCLUDE_AUDIO_TEST )
endif ( NOT EXISTS ${PROJECT_SOURCE_DIR}/assets/test_music.ogg )

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
