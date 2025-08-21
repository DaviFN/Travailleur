cmake_minimum_required(VERSION 3.24)

set(SOURCES
    src/EasyBMP.cpp
)

add_library(EasyBMP STATIC ${SOURCES})

target_include_directories(
    EasyBMP
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

install(TARGETS EasyBMP DESTINATION "${INSTALLATION_DIRECTORY}")