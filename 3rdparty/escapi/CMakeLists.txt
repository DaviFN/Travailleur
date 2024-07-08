cmake_minimum_required(VERSION 3.24)

set(SOURCES
    capture.cpp
    conversion.cpp
    escapi_dll.cpp
    interface.cpp
    videobufferlock.cpp
)

add_library(escapi SHARED ${SOURCES})

target_include_directories(
    escapi
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
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