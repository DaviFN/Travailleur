cmake_minimum_required(VERSION 3.24)

set(SOURCES
    src/capture.cpp
    src/conversion.cpp
    src/escapi_dll.cpp
    src/interface.cpp
    src/videobufferlock.cpp
)

add_library(escapi SHARED ${SOURCES})

target_include_directories(
    escapi
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    escapi
    PUBLIC
    mfplat.lib
    mf.lib
    mfreadwrite.lib
    mfuuid.lib
    shlwapi.lib
    kernel32.lib
    user32.lib
    gdi32.lib
    winspool.lib
    comdlg32.lib
    advapi32.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    odbc32.lib
    odbccp32.lib
)

install(TARGETS escapi DESTINATION "${INSTALLATION_DIRECTORY}")