#pragma once

#include "Engine/Export.hpp"

#include <cstddef>

struct ID3D11Device;

namespace Engine {

class Camera;
class InputManager;
class StateManager;
class Time;
class UISystem;
class World;

struct PostProcessSettings;

struct ApplicationConfig {
    const char* windowTitle = "TRON Engine";
    int width = 1280;
    int height = 720;
};

class ENGINE_API Application {
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    bool Initialize(const ApplicationConfig& config = {});
    void Run();
    void Shutdown();

    World& GetWorld();
    Camera& GetCamera();
    StateManager& GetStateManager();
    InputManager& GetInput();
    UISystem& GetUI();
    Time& GetTime();
    ID3D11Device* GetDevice() const;

    void SetWindowTitle(const char* title);
    void SetClearColor(float r, float g, float b, float a = 1.0f);
    void RequestQuit();
    void ResetPhysics();
    void DiscardNextFrameDelta();

    void SetPostProcess(float saturation, float brightness, float contrast);
    void GetPostProcess(float& saturation, float& brightness, float& contrast) const;

    unsigned int GetLastDrawnCount() const;
    unsigned int GetLastCulledCount() const;
    int GetWidth() const;
    int GetHeight() const;

private:
    bool CreateAppWindow(const ApplicationConfig& config);
    void ProcessPendingMessages(bool& running);
    void Update(float deltaSeconds);
    void RenderFrame();
    static bool ResolveShaderPath(char* outPath, size_t outSize);

    struct Impl;
    Impl* m_impl;
};

} // namespace Engine
