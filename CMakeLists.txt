cmake_minimum_required(VERSION 3.3)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 90)
set(VERSION_REVISION 0)
set(VERSION_STATUS "-dev")
string(TIMESTAMP VERSION_YEAR "%Y")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(VERSION_BUILD ".dbg" )
endif()

find_package(Git)
if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} status
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        ERROR_VARIABLE RESULT_STRING
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    string(LENGTH "${RESULT_STRING}" LENGTH_RESULT_STRING)

    if(${LENGTH_RESULT_STRING} EQUAL 0)

        execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --format=%H
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        string(SUBSTRING ${GIT_COMMIT_HASH} 0 7 GIT_COMMIT_HASH)
        set(VERSION_HASH " (${GIT_COMMIT_HASH})" )

        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list HEAD --count
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE VERSION_REVISION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

    endif()
endif()

project(TIC-80 VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION} LANGUAGES C CXX)
message("Building for target : ${CMAKE_SYSTEM_NAME}")

message("PROJECT_VERSION: ${PROJECT_VERSION}${VERSION_STATUS}")
message("VERSION_HASH: ${VERSION_HASH}")

configure_file("${PROJECT_SOURCE_DIR}/version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/version.h")

option(BUILD_SDL "SDL Enabled" ON)
option(BUILD_PLAYER "Build standalone players" ON)

if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN AND NOT ANDROID AND NOT N3DS)
    set(LINUX TRUE)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (NOT DEFINED CMAKE_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(MSVC)

    add_definitions("/D\"_CRT_SECURE_NO_WARNINGS\"")
    add_definitions("/D\"_CRT_NONSTDC_NO_DEPRECATE\"")

    foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
        string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
        set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
        set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
        set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin )

        # use static runtime
        if(CMAKE_C_FLAGS_${OUTPUTCONFIG} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" CMAKE_C_FLAGS_${OUTPUTCONFIG} "${CMAKE_C_FLAGS_${OUTPUTCONFIG}}")
        endif()

        if(CMAKE_CXX_FLAGS_${OUTPUTCONFIG} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_${OUTPUTCONFIG} "${CMAKE_CXX_FLAGS_${OUTPUTCONFIG}}")
        endif()
    endforeach()
    
else()

    set(CMAKE_C_STANDARD 99)
endif()

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/vendor)

################################
# LUA
################################

set(LUA_DIR ${THIRDPARTY_DIR}/lua)
set(LUA_SRC 
    ${LUA_DIR}/lapi.c
    ${LUA_DIR}/lcode.c
    ${LUA_DIR}/lctype.c
    ${LUA_DIR}/ldebug.c
    ${LUA_DIR}/ldo.c
    ${LUA_DIR}/ldump.c
    ${LUA_DIR}/lfunc.c
    ${LUA_DIR}/lgc.c
    ${LUA_DIR}/llex.c
    ${LUA_DIR}/lmem.c
    ${LUA_DIR}/lobject.c
    ${LUA_DIR}/lopcodes.c
    ${LUA_DIR}/lparser.c
    ${LUA_DIR}/lstate.c
    ${LUA_DIR}/lstring.c
    ${LUA_DIR}/ltable.c
    ${LUA_DIR}/ltm.c
    ${LUA_DIR}/lundump.c
    ${LUA_DIR}/lvm.c
    ${LUA_DIR}/lzio.c
    ${LUA_DIR}/lauxlib.c
    ${LUA_DIR}/lbaselib.c
    ${LUA_DIR}/lcorolib.c
    ${LUA_DIR}/ldblib.c
    ${LUA_DIR}/liolib.c
    ${LUA_DIR}/lmathlib.c
    ${LUA_DIR}/loslib.c
    ${LUA_DIR}/lstrlib.c
    ${LUA_DIR}/ltablib.c
    ${LUA_DIR}/lutf8lib.c
    ${LUA_DIR}/loadlib.c
    ${LUA_DIR}/linit.c
)

add_library(lua STATIC ${LUA_SRC})

target_compile_definitions(lua PRIVATE LUA_COMPAT_5_2)
target_include_directories(lua INTERFACE ${THIRDPARTY_DIR}/lua)

################################
# Blipbuf
################################

add_library(blipbuf STATIC ${THIRDPARTY_DIR}/blip-buf/blip_buf.c)
target_include_directories(blipbuf INTERFACE ${THIRDPARTY_DIR}/blip-buf)

################################
# TIC-80 core
################################

set(TIC80CORE_DIR ${CMAKE_SOURCE_DIR}/src)
set(TIC80CORE_SRC
    ${TIC80CORE_DIR}/core/core.c
    ${TIC80CORE_DIR}/core/draw.c
    ${TIC80CORE_DIR}/core/io.c
    ${TIC80CORE_DIR}/core/sound.c
    ${TIC80CORE_DIR}/api/lua.c 
    ${TIC80CORE_DIR}/tic.c
    ${TIC80CORE_DIR}/cart.c
    ${TIC80CORE_DIR}/tools.c 
    ${TIC80CORE_DIR}/tilesheet.c 
)

add_library(tic80core STATIC ${TIC80CORE_SRC})

target_include_directories(tic80core 
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(tic80core lua blipbuf)

target_compile_definitions(tic80core PRIVATE TIC_BUILD_WITH_LUA)

if(LINUX)
    target_link_libraries(tic80core m)
endif()

################################
# SDL2
################################
if(BUILD_SDL AND NOT EMSCRIPTEN AND NOT RPI)

    if(WIN32)
        set(HAVE_LIBC TRUE)
    endif()

    if(ANDROID)
        include_directories(${ANDROID_NDK}/sources/android/cpufeatures)
    endif()
    
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)

    add_subdirectory(${THIRDPARTY_DIR}/sdl2)

endif()

################################
# SDL2 standalone cart player
################################

if(BUILD_SDL AND BUILD_PLAYER)

    add_executable(player-sdl WIN32 ${CMAKE_SOURCE_DIR}/src/system/sdl/player.c)

    target_include_directories(player-sdl PRIVATE 
        ${THIRDPARTY_DIR}/sdl2/include 
        ${CMAKE_SOURCE_DIR}/include 
        ${CMAKE_SOURCE_DIR}/src)

    if(MINGW)
        target_link_libraries(player-sdl mingw32)
        target_link_options(player-sdl PRIVATE -static)
    endif()

    target_link_libraries(player-sdl tic80core SDL2-static SDL2main)
endif()

