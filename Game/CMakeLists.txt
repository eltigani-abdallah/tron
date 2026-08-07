add_executable(Game
    src/main.cpp
)

target_include_directories(Game
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Game
    PRIVATE
        Tron::Engine
)

target_compile_definitions(Game
    PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_options(Game PRIVATE /W4 /permissive-)

set_target_properties(Game PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
