FILE(GLOB SOURCES
    Core/*.cpp
    Common/*.cpp
    ImGuiWrappers/*.cpp
    3rdparty/escapi/escapi.cpp
)

message(STATUS "SOURCES: ${SOURCES}")

#set(SOURCES
#    Core/src/TravailleurContext.cpp
#    Core/src/TravailleurContext.cpp
#    Core/src/TravailleurContext.cpp
#)

add_library(TravailleurFramework STATIC ${SOURCES})

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    find_library(XTST_LIB Xtst REQUIRED)
    target_link_libraries(TravailleurFramework PUBLIC ${X11_LIBRARIES} ${XTST_LIB})
    target_include_directories(TravailleurFramework PUBLIC ${X11_INCLUDE_DIR})
endif()

#include_directories(
#    TravailleurFramework
#    PUBLIC
#)

#set(VCPKG_TARGET_TRIPLET, "x64-windows")
#set(CMAKE_TOOLCHAIN_FILE, "C:\\devtools\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")

#set(OpenCV_DIR "C:\\cmake-libs\\opencv\\build")

find_package(OpenCV REQUIRED)

#find_package(Tesseract CONFIG REQUIRED)
find_package(Tesseract REQUIRED)


#start of VLD related stuff

#comment out to disable vld build
#set(VLD_BUILD_ENABLED "VLD_BUILD_ENABLED")

if(VLD_BUILD_ENABLED)

	find_library(VLD_LIB vld HINTS "C:/Program Files (x86)/Visual Leak Detector/lib/Win64/")
	find_path(VLD_H_PATH vld.h HINTS "C:/Program Files (x86)/Visual Leak Detector/include")
	set(VLD_BIN_PATH "C:/Program Files (x86)/Visual Leak Detector/bin/Win64")
	message(STATUS "VLD_LIB: ${VLD_LIB}")
	message(STATUS "VLD_H_PATH: ${VLD_H_PATH}")
	message(STATUS "VLD_BIN_PATH: ${VLD_BIN_PATH}")
	if(VLD_LIB)
		add_compile_definitions("VLD_BUILD")
		target_include_directories(TravailleurFramework PUBLIC ${VLD_H_PATH})
		target_link_libraries(TravailleurFramework PUBLIC ${VLD_LIB})
		
		message("Enabling Visual Leak Detector Done")
	else()
		message("Could not enable Visual Leak Detector")
	endif()
	
	add_custom_command(
		TARGET TravailleurFramework POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${VLD_BIN_PATH}/vld_x64.dll"
			${INSTALLATION_DIRECTORY}
	)
	
	add_custom_command(
		TARGET TravailleurFramework POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${VLD_BIN_PATH}/dbghelp.dll"
			${INSTALLATION_DIRECTORY}
	)
	
	add_custom_command(
		TARGET TravailleurFramework POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${VLD_BIN_PATH}/Microsoft.DTfW.DHL.manifest"
			${INSTALLATION_DIRECTORY}
	)
	
	add_custom_command(
		TARGET TravailleurFramework POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${VLD_BIN_PATH}/vld_x64.pdb"
			${INSTALLATION_DIRECTORY}
	)
endif()

#obs: change ReportFile to ReportFile = memory_report.txt and ReportTo to ReportTo = file
#in vld.ini (where vld was installed)

#end of VLD related stuff


target_include_directories(
    TravailleurFramework
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${Tesseract_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/3rdparty/screen_capture_lite/include
)

target_link_libraries(
    TravailleurFramework
    PUBLIC
	imgui::imgui
	screen_capture_lite_shared
    gl3w
    ${SDL3_LIBRARIES}
    EasyBMP
    md5
    ${OpenCV_LIBS}
    PRIVATE
    Tesseract::libtesseract
)

# copy opencv libs into install folder
#ADD_CUSTOM_COMMAND (
#        TARGET ${trgt}
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND}
#        ARGS -E copy "${OPENCVDIR/bin}/${libname}.dll" ${CMAKE_CURRENT_BINARY_DIR}
#)

#ADD_CUSTOM_COMMAND (
#        TARGET ${TravailleurFramework}
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND}
#        ARGS -E copy "${OPENCVDIR/bin}/${libname}.dll" ${CMAKE_CURRENT_BINARY_DIR}
#)

#install(FILES ${OpenCV_LIBS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#MACRO (COPY_DLL trgt libname)
#        ADD_CUSTOM_COMMAND (
#        TARGET ${trgt}
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND}
#        ARGS -E copy "${OPENCVDIR/bin}/${libname}.dll" ${CMAKE_CURRENT_BINARY_DIR}
#    )
#ENDMACRO (COPY_DLL)

#COPY_DLL

#install(
#    TARGETS TravailleurFramework
#    RUNTIME_DEPENDENCIES
#)

#add_custom_command(travailleur exe POST_BUILD
#  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:exe> $<TARGET_FILE_DIR:exe>
#  COMMAND_EXPAND_LISTS
#)

install(TARGETS TravailleurFramework DESTINATION "${INSTALLATION_DIRECTORY}")

#add_custom_command(
#    TARGET TravailleurFramework POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        ${OpenCV_LIBS}
#        ${INSTALLATION_DIRECTORY}
#)

#install(FILES $<TARGET_RUNTIME_DLLS:your_exe_here> TYPE BIN)


#add_custom_command(
#    TARGET TravailleurFramework POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        "${ESCAPI_DIR_PATH}/lib/x64/escapi.dll"
#        ${INSTALLATION_DIRECTORY}
#)

#set_target_properties(travailleur PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${INSTALLATION_DIRECTORY})

#add_custom_command(TARGET RouletteProject POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    $<TARGET_FILE:OpenCV> $<TARGET_FILE_DIR:RouletteProject>
#)

set(VKPKG_BINARIES_PATH ${_VCPKG_INSTALLED_DIR}/x64-windows/bin)

message(STATUS "VKPKG_BINARIES_PATH: ${VKPKG_BINARIES_PATH}")

file(GLOB_RECURSE VCPKG_ALL_DLLS ${VKPKG_BINARIES_PATH}/*.dll)

message(STATUS "VCPKG_ALL_DLLS: ${VCPKG_ALL_DLLS}")


foreach(VCPKG_DLL_FILE ${VCPKG_ALL_DLLS})
    add_custom_command(TARGET TravailleurFramework POST_BUILD
    	COMMAND ${CMAKE_COMMAND} -E copy_if_different
    	${VCPKG_DLL_FILE} ${INSTALLATION_DIRECTORY}
    )
endforeach(VCPKG_DLL_FILE)



add_subdirectory("Common/unittests")
add_subdirectory("Core/unittests")