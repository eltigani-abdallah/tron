#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Render/Camera.hpp"
#include "Engine/Render/Mesh.hpp"
#include "Engine/Render/Shader.hpp"
#include "Engine/Render/Vertex.hpp"

struct HWND__;
typedef HWND__* HWND;
struct ID3D11Device;

namespace Engine {

struct PostProcessSettings {
    float saturation = 1.0f;
    float brightness = 0.0f;
    float contrast = 1.0f;
};

class ENGINE_API Renderer {
public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool Initialize(HWND hwnd, int width, int height, const char* basicShaderPath);
    void Shutdown();

    void BeginFrame(float r, float g, float b, float a = 1.0f);
    void Draw(const Mesh& mesh, const Transform& transform, const Camera& camera, const Vec3& tint);
    void DrawWorldTriangles(
        const Vertex* vertices,
        unsigned int vertexCount,
        const Mat4& viewProjection,
        bool depthTest = true);
    void ApplyPostProcess();
    void EndFrame();

    void SetPostProcessSettings(const PostProcessSettings& settings);
    PostProcessSettings GetPostProcessSettings() const;

    ID3D11Device* GetDevice() const;
    float AspectRatio() const;

    bool IsInitialized() const { return m_initialized; }

private:
    bool CreateDeviceAndSwapChain(HWND hwnd);
    bool CreateDepthStencil();
    bool CreateSceneTargets();
    bool CreatePipelineState();
    bool CreatePostProcessResources(const char* basicShaderPath);
    static bool MakeSiblingShaderPath(const char* basicShaderPath, const char* fileName, char* outPath, size_t outSize);

    struct Impl;
    Impl* m_impl;
    bool m_initialized = false;
};

} // namespace Engine
