cmake_minimum_required(VERSION 3.24)

FILE(GLOB SOURCES
    *.cpp
)

#project(sample CXX)
#enable_testing()
find_package(GTest REQUIRED)
add_executable(TravailleurFrameworkCommonUnitTests ${SOURCES})
target_link_libraries(TravailleurFrameworkCommonUnitTests TravailleurFramework GTest::GTest GTest::Main)

set_target_properties(TravailleurFrameworkCommonUnitTests PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${INSTALLATION_DIRECTORY})