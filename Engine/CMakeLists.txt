add_library(Engine SHARED
    src/Core/Application.cpp
    src/Core/Script.cpp
    src/Core/ScriptManager.cpp
    src/ECS/World.cpp
    src/Math/Vec3.cpp
    src/Math/Mat4.cpp
    src/Math/Quaternion.cpp
    src/Math/Transform.cpp
    src/Render/Mesh.cpp
    src/Render/Shader.cpp
    src/Render/Camera.cpp
    src/Render/Renderer.cpp
    src/Render/Frustum.cpp
    src/States/State.cpp
    src/States/StateManager.cpp
    src/Input/InputManager.cpp
    src/Physics/SpatialPartition.cpp
    src/Physics/PhysicsSystem.cpp
    src/Particles/ParticleSystem.cpp
    src/UI/UISystem.cpp
    src/Time/Time.cpp
)

add_library(Tron::Engine ALIAS Engine)

target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(Engine
    PRIVATE
        ENGINE_EXPORTS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_link_libraries(Engine
    PUBLIC
        d3d11
        dxgi
        d3dcompiler
)

target_compile_options(Engine PRIVATE /W4 /permissive-)

add_custom_command(TARGET Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders
            $<TARGET_FILE_DIR:Engine>/shaders
    COMMENT "Copying Engine shaders next to Engine.dll"
)
