cmake_minimum_required(VERSION 3.24)

set(SOURCES
    src/md5.cpp
)

add_library(md5 ${SOURCES})

target_include_directories(
    md5
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

install(TARGETS md5 DESTINATION "${INSTALLATION_DIRECTORY}")