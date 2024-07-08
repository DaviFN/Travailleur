cmake_minimum_required(VERSION 3.24)

set(SOURCES
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    backends/imgui_impl_sdl.cpp
    backends/imgui_impl_opengl3.cpp
)

add_library(imgui ${SOURCES})

target_include_directories(
    imgui
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends"
)

target_link_libraries(
    imgui
    ${SDL2_LIBRARIES}
    Opengl32.lib
)

install(TARGETS imgui DESTINATION "${INSTALLATION_DIRECTORY}")