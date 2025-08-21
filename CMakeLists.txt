cmake_minimum_required(VERSION 3.5)

#set(CMAKE_FIND_DEBUG_MODE ON) << can be used to find out where libraries are located

SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
project(travailleur)

set(CMAKE_CXX_STANDARD 20)

set(INSTALLATION_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin/${CMAKE_BUILD_TYPE}")

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${INSTALLATION_DIRECTORY})

set(DCMAKE_BUILD_TYPE "ReleaseWithDebInfo")

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

message(STATUS "INSTALLATION_DIRECTORY: ${INSTALLATION_DIRECTORY}")
message(STATUS "CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "CMAKE_FIND_PACKAGE_REDIRECTS_DIR: ${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}")

find_package(imgui CONFIG REQUIRED)

add_subdirectory("3rdparty/screen_capture_lite")
add_subdirectory("3rdparty/gl3w")
add_subdirectory("3rdparty/EasyBMP")
add_subdirectory("3rdparty/md5")

add_subdirectory("TravailleurFramework")

add_subdirectory("DummyProject")