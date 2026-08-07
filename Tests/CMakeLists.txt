add_executable(TronTests
    src/main.cpp
    src/TestMath.cpp
    src/TestTime.cpp
    src/TestSpatial.cpp
    src/TestWorld.cpp
    src/TestGameSession.cpp
)

target_include_directories(TronTests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/Game/include
)

target_link_libraries(TronTests
    PRIVATE
        Tron::Engine
)

target_compile_definitions(TronTests
    PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_options(TronTests PRIVATE /W4 /permissive-)

set_target_properties(TronTests PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

enable_testing()
add_test(NAME TronUnitTests COMMAND TronTests)
