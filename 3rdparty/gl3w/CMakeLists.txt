cmake_minimum_required(VERSION 3.24)

set(SOURCES
    gl3w.c
)

add_library(gl3w ${SOURCES})

target_include_directories(
    gl3w
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

install(TARGETS gl3w DESTINATION "${INSTALLATION_DIRECTORY}")