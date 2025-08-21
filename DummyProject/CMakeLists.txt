FILE(GLOB SOURCES *.cpp)

message(STATUS "SOURCES: ${SOURCES}")

add_executable(DummyProject ${SOURCES})

target_link_libraries(
    DummyProject
    PRIVATE
    TravailleurFramework
)

install(TARGETS DummyProject DESTINATION "${INSTALLATION_DIRECTORY}")

set_target_properties(DummyProject PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${INSTALLATION_DIRECTORY})