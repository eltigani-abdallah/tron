#include "Engine/Core/Application.hpp"

#include "Engine/ECS/Components.hpp"
#include "Engine/ECS/World.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Particles/ParticleSystem.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Render/Camera.hpp"
#include "Engine/Render/Frustum.hpp"
#include "Engine/Render/Renderer.hpp"
#include "Engine/States/StateManager.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/UI/UISystem.hpp"

#include <Windows.h>

#include <chrono>
#include <cstring>
#include <stdexcept>
#include <string>

namespace Engine {
namespace {

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

std::wstring ToWide(const char* text) {
    if (text == nullptr || text[0] == '\0') {
        return L"TRON Engine";
    }

    const int size = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);
    std::wstring result(static_cast<size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text, -1, result.data(), size);
    if (!result.empty() && result.back() == L'\0') {
        result.pop_back();
    }
    return result;
}

} // namespace

struct Application::Impl {
    HINSTANCE instance = nullptr;
    HWND hwnd = nullptr;
    int width = 1280;
    int height = 720;
    bool initialized = false;
    bool quitRequested = false;
    bool discardNextDelta = false;

    float clearColor[4] = {0.02f, 0.05f, 0.12f, 1.0f};

    Renderer renderer;
    World world;
    Camera camera;
    StateManager stateManager;
    InputManager input;
    PhysicsSystem physics;
    ParticleSystem particles;
    UISystem ui;
    Time time;

    unsigned int lastDrawnCount = 0;
    unsigned int lastCulledCount = 0;
};

Application::Application()
    : m_impl(new Impl()) {
}

Application::~Application() {
    Shutdown();
    delete m_impl;
    m_impl = nullptr;
}

bool Application::Initialize(const ApplicationConfig& config) {
    if (m_impl->initialized) {
        return true;
    }

    m_impl->width = config.width > 0 ? config.width : 1280;
    m_impl->height = config.height > 0 ? config.height : 720;
    m_impl->quitRequested = false;

    if (!CreateAppWindow(config)) {
        return false;
    }

    char shaderPath[MAX_PATH] = {};
    if (!ResolveShaderPath(shaderPath, MAX_PATH)) {
        Shutdown();
        return false;
    }

    if (!m_impl->renderer.Initialize(m_impl->hwnd, m_impl->width, m_impl->height, shaderPath)) {
        Shutdown();
        return false;
    }

    m_impl->input.SetWindow(m_impl->hwnd);
    m_impl->world.SetInput(&m_impl->input);
    m_impl->world.SetCamera(&m_impl->camera);
    m_impl->world.SetTime(&m_impl->time);
    m_impl->world.SetDevice(m_impl->renderer.GetDevice());
    m_impl->ui.SetScreenSize(m_impl->width, m_impl->height);

    m_impl->camera.aspect = m_impl->renderer.AspectRatio();
    m_impl->camera.fovYDegrees = 60.0f;
    m_impl->camera.transform.position = {0.0f, 1.2f, -4.0f};
    m_impl->camera.LookAt(Vec3::Zero());

    m_impl->initialized = true;
    return true;
}

void Application::Run() {
    if (!m_impl->initialized) {
        throw std::runtime_error("Application::Run called before Initialize");
    }

    ShowWindow(m_impl->hwnd, SW_SHOW);
    UpdateWindow(m_impl->hwnd);

    using Clock = std::chrono::steady_clock;
    auto previous = Clock::now();

    bool running = true;
    while (running && !m_impl->quitRequested) {
        ProcessPendingMessages(running);
        if (!running || m_impl->quitRequested) {
            break;
        }

        const auto now = Clock::now();
        float deltaSeconds = std::chrono::duration<float>(now - previous).count();
        previous = now;

        if (m_impl->discardNextDelta) {
            m_impl->discardNextDelta = false;
            deltaSeconds = 0.0f;
        }

        Update(deltaSeconds);
        RenderFrame();
    }
}

void Application::Shutdown() {
    if (m_impl == nullptr) {
        return;
    }

    m_impl->stateManager.Clear();
    m_impl->ui.Clear();
    m_impl->physics.Reset();
    m_impl->world.Clear();
    m_impl->renderer.Shutdown();

    if (m_impl->hwnd != nullptr) {
        DestroyWindow(m_impl->hwnd);
        m_impl->hwnd = nullptr;
    }

    if (m_impl->instance != nullptr) {
        UnregisterClassW(L"TronEngineWindowClass", m_impl->instance);
        m_impl->instance = nullptr;
    }

    m_impl->initialized = false;
    m_impl->quitRequested = false;
}

World& Application::GetWorld() {
    return m_impl->world;
}

Camera& Application::GetCamera() {
    return m_impl->camera;
}

StateManager& Application::GetStateManager() {
    return m_impl->stateManager;
}

InputManager& Application::GetInput() {
    return m_impl->input;
}

UISystem& Application::GetUI() {
    return m_impl->ui;
}

Time& Application::GetTime() {
    return m_impl->time;
}

ID3D11Device* Application::GetDevice() const {
    return m_impl->renderer.GetDevice();
}

void Application::SetWindowTitle(const char* title) {
    if (m_impl->hwnd != nullptr) {
        SetWindowTextW(m_impl->hwnd, ToWide(title).c_str());
    }
}

void Application::SetClearColor(float r, float g, float b, float a) {
    m_impl->clearColor[0] = r;
    m_impl->clearColor[1] = g;
    m_impl->clearColor[2] = b;
    m_impl->clearColor[3] = a;
}

void Application::RequestQuit() {
    m_impl->quitRequested = true;
}

void Application::ResetPhysics() {
    m_impl->physics.Reset();
}

void Application::DiscardNextFrameDelta() {
    m_impl->discardNextDelta = true;
}

void Application::SetPostProcess(float saturation, float brightness, float contrast) {
    PostProcessSettings settings;
    settings.saturation = saturation;
    settings.brightness = brightness;
    settings.contrast = contrast;
    m_impl->renderer.SetPostProcessSettings(settings);
}

void Application::GetPostProcess(float& saturation, float& brightness, float& contrast) const {
    const PostProcessSettings settings = m_impl->renderer.GetPostProcessSettings();
    saturation = settings.saturation;
    brightness = settings.brightness;
    contrast = settings.contrast;
}

unsigned int Application::GetLastDrawnCount() const {
    return m_impl->lastDrawnCount;
}

unsigned int Application::GetLastCulledCount() const {
    return m_impl->lastCulledCount;
}

int Application::GetWidth() const {
    return m_impl->width;
}

int Application::GetHeight() const {
    return m_impl->height;
}

bool Application::CreateAppWindow(const ApplicationConfig& config) {
    m_impl->instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_impl->instance;
    wc.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)); // IDC_ARROW
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = L"TronEngineWindowClass";

    if (RegisterClassExW(&wc) == 0) {
        const DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
    }

    RECT rect = {0, 0, m_impl->width, m_impl->height};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    const std::wstring title = ToWide(config.windowTitle);
    m_impl->hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        m_impl->instance,
        nullptr);

    return m_impl->hwnd != nullptr;
}

void Application::ProcessPendingMessages(bool& running) {
    MSG msg = {};
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            running = false;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void Application::Update(float deltaSeconds) {
    m_impl->time.Tick(deltaSeconds);

    m_impl->input.Update();
    m_impl->ui.Update(m_impl->input);

    // State timers (splash/menu/gameover) use real time.
    m_impl->stateManager.Update(m_impl->time.RealDelta());

    m_impl->physics.Update(m_impl->world);
    m_impl->world.LateUpdateScripts(m_impl->time.ScaledDelta());
    m_impl->particles.Update(m_impl->world, m_impl->time.ScaledDelta());
    m_impl->world.FlushDestroyed();
}

void Application::RenderFrame() {
    m_impl->renderer.BeginFrame(
        m_impl->clearColor[0],
        m_impl->clearColor[1],
        m_impl->clearColor[2],
        m_impl->clearColor[3]);

    const Frustum frustum = Frustum::FromViewProjection(m_impl->camera.ViewProjectionMatrix());
    m_impl->lastDrawnCount = 0;
    m_impl->lastCulledCount = 0;

    const auto& meshes = m_impl->world.Components<MeshComponent>();
    for (const auto& [entity, meshComponent] : meshes) {
        const TransformComponent* transform = m_impl->world.GetComponent<TransformComponent>(entity);
        if (transform == nullptr || !meshComponent.mesh.IsValid()) {
            continue;
        }

        AABB bounds;
        if (const ColliderComponent* collider = m_impl->world.GetComponent<ColliderComponent>(entity)) {
            bounds = ComputeWorldAABB(transform->local, *collider);
        } else {
            bounds = ComputeDefaultMeshAABB(transform->local, 0.5f);
        }

        if (!frustum.IntersectsAABB(bounds)) {
            ++m_impl->lastCulledCount;
            continue;
        }

        m_impl->renderer.Draw(meshComponent.mesh, transform->local, m_impl->camera, meshComponent.tint);
        ++m_impl->lastDrawnCount;
    }

    m_impl->particles.Render(m_impl->renderer, m_impl->camera, m_impl->world);
    m_impl->renderer.ApplyPostProcess();
    m_impl->ui.Render(m_impl->renderer);
    m_impl->renderer.EndFrame();
}

bool Application::ResolveShaderPath(char* outPath, size_t outSize) {
    if (outPath == nullptr || outSize == 0) {
        return false;
    }

    char modulePath[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameA(nullptr, modulePath, MAX_PATH);
    if (length == 0 || length >= MAX_PATH) {
        return false;
    }

    std::string directory(modulePath);
    const size_t slash = directory.find_last_of("\\/");
    if (slash != std::string::npos) {
        directory.resize(slash + 1);
    }

    const std::string candidates[] = {
        directory + "shaders\\Basic.hlsl",
        directory + "..\\shaders\\Basic.hlsl",
        "shaders\\Basic.hlsl",
        "Engine\\shaders\\Basic.hlsl",
    };

    for (const std::string& candidate : candidates) {
        const DWORD attrs = GetFileAttributesA(candidate.c_str());
        if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0) {
            if (candidate.size() + 1 > outSize) {
                return false;
            }
            memcpy(outPath, candidate.c_str(), candidate.size() + 1);
            return true;
        }
    }

    return false;
}

} // namespace Engine
