cmake_minimum_required(VERSION 3.24)

#set(CMAKE_FIND_DEBUG_MODE ON) << can be used to find out where libraries are located

SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
project(travailleur)

set(CMAKE_CXX_STANDARD 20)

set(INSTALLATION_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/${CMAKE_BUILD_TYPE}")

#set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${INSTALLATION_DIRECTORY})

message(STATUS "SDL2_LIBDIR: ${SDL2_LIBDIR}")

set(DCMAKE_BUILD_TYPE "ReleaseWithDebInfo")

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

#set(SDL2_DIR "./third-party/sdl2/cmake/")
#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/third-party/sdl2/cmake")
#find_package(SDL2 REQUIRED)
set(SDL2_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}/3rdparty/sdl2")
include("${SDL2_DIR_PATH}/cmake/sdl2-config.cmake")
include_directories(${SDL2_INCLUDE_DIRS})

message(STATUS "INSTALLATION_DIRECTORY: ${INSTALLATION_DIRECTORY}")
message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_LIBRARIES: ${SDL2_LIBRARIES}")
message(STATUS "CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "CMAKE_FIND_PACKAGE_REDIRECTS_DIR: ${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}")

#set(ESCAPI_DIR_PATH "${CMAKE_CURRENT_LIST_DIR}/3rdparty/escapi")

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/../../buildoutput/${CMAKE_BUILD_TYPE}/bin")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/../../buildoutput/${CMAKE_BUILD_TYPE}/lib")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/../../buildoutput/${CMAKE_BUILD_TYPE}/lib")

add_subdirectory("TravailleurFramework")

add_subdirectory("3rdparty/escapi")
add_subdirectory("3rdparty/imgui")
add_subdirectory("3rdparty/gl3w")
add_subdirectory("3rdparty/EasyBMP")
add_subdirectory("3rdparty/md5")

add_subdirectory("DummyProject")

#include("third-party/SDL2.cmake")