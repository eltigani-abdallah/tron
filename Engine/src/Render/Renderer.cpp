#include "Engine/Render/Renderer.hpp"

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#include <cstring>
#include <string>

namespace Engine {

struct Renderer::Impl {
    int width = 1280;
    int height = 720;

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;

    ID3D11RenderTargetView* backBufferRTV = nullptr;

    ID3D11Texture2D* sceneTexture = nullptr;
    ID3D11RenderTargetView* sceneRTV = nullptr;
    ID3D11ShaderResourceView* sceneSRV = nullptr;

    ID3D11Texture2D* depthStencilBuffer = nullptr;
    ID3D11DepthStencilView* depthStencilView = nullptr;
    ID3D11RasterizerState* rasterizerState = nullptr;
    ID3D11RasterizerState* particleRasterizerState = nullptr;
    ID3D11DepthStencilState* depthStencilState = nullptr;
    ID3D11DepthStencilState* particleDepthState = nullptr;
    ID3D11BlendState* alphaBlendState = nullptr;
    ID3D11Buffer* dynamicVertexBuffer = nullptr;
    UINT dynamicVertexCapacity = 0;

    Shader shader;

    ID3D11VertexShader* postVS = nullptr;
    ID3D11PixelShader* postPS = nullptr;
    ID3D11Buffer* postConstantBuffer = nullptr;
    ID3D11SamplerState* postSampler = nullptr;

    PostProcessSettings postSettings;
};

Renderer::Renderer()
    : m_impl(new Impl()) {
}

Renderer::~Renderer() {
    Shutdown();
    delete m_impl;
    m_impl = nullptr;
}

bool Renderer::Initialize(HWND hwnd, int width, int height, const char* basicShaderPath) {
    if (m_initialized) {
        return true;
    }

    m_impl->width = width > 0 ? width : 1280;
    m_impl->height = height > 0 ? height : 720;

    if (!CreateDeviceAndSwapChain(hwnd)) {
        Shutdown();
        return false;
    }
    if (!CreateSceneTargets()) {
        Shutdown();
        return false;
    }
    if (!CreateDepthStencil()) {
        Shutdown();
        return false;
    }
    if (!CreatePipelineState()) {
        Shutdown();
        return false;
    }
    if (!m_impl->shader.LoadFromFile(m_impl->device, basicShaderPath)) {
        Shutdown();
        return false;
    }
    if (!CreatePostProcessResources(basicShaderPath)) {
        Shutdown();
        return false;
    }

    m_initialized = true;
    return true;
}

void Renderer::Shutdown() {
    if (m_impl == nullptr) {
        return;
    }

    m_impl->shader.Release();

    if (m_impl->postSampler != nullptr) {
        m_impl->postSampler->Release();
        m_impl->postSampler = nullptr;
    }
    if (m_impl->postConstantBuffer != nullptr) {
        m_impl->postConstantBuffer->Release();
        m_impl->postConstantBuffer = nullptr;
    }
    if (m_impl->postPS != nullptr) {
        m_impl->postPS->Release();
        m_impl->postPS = nullptr;
    }
    if (m_impl->postVS != nullptr) {
        m_impl->postVS->Release();
        m_impl->postVS = nullptr;
    }

    if (m_impl->dynamicVertexBuffer != nullptr) {
        m_impl->dynamicVertexBuffer->Release();
        m_impl->dynamicVertexBuffer = nullptr;
    }
    m_impl->dynamicVertexCapacity = 0;

    if (m_impl->alphaBlendState != nullptr) {
        m_impl->alphaBlendState->Release();
        m_impl->alphaBlendState = nullptr;
    }
    if (m_impl->particleDepthState != nullptr) {
        m_impl->particleDepthState->Release();
        m_impl->particleDepthState = nullptr;
    }
    if (m_impl->particleRasterizerState != nullptr) {
        m_impl->particleRasterizerState->Release();
        m_impl->particleRasterizerState = nullptr;
    }
    if (m_impl->depthStencilState != nullptr) {
        m_impl->depthStencilState->Release();
        m_impl->depthStencilState = nullptr;
    }
    if (m_impl->rasterizerState != nullptr) {
        m_impl->rasterizerState->Release();
        m_impl->rasterizerState = nullptr;
    }
    if (m_impl->depthStencilView != nullptr) {
        m_impl->depthStencilView->Release();
        m_impl->depthStencilView = nullptr;
    }
    if (m_impl->depthStencilBuffer != nullptr) {
        m_impl->depthStencilBuffer->Release();
        m_impl->depthStencilBuffer = nullptr;
    }
    if (m_impl->sceneSRV != nullptr) {
        m_impl->sceneSRV->Release();
        m_impl->sceneSRV = nullptr;
    }
    if (m_impl->sceneRTV != nullptr) {
        m_impl->sceneRTV->Release();
        m_impl->sceneRTV = nullptr;
    }
    if (m_impl->sceneTexture != nullptr) {
        m_impl->sceneTexture->Release();
        m_impl->sceneTexture = nullptr;
    }
    if (m_impl->backBufferRTV != nullptr) {
        m_impl->backBufferRTV->Release();
        m_impl->backBufferRTV = nullptr;
    }
    if (m_impl->swapChain != nullptr) {
        m_impl->swapChain->Release();
        m_impl->swapChain = nullptr;
    }
    if (m_impl->context != nullptr) {
        m_impl->context->ClearState();
        m_impl->context->Flush();
        m_impl->context->Release();
        m_impl->context = nullptr;
    }
    if (m_impl->device != nullptr) {
        m_impl->device->Release();
        m_impl->device = nullptr;
    }

    m_initialized = false;
}

void Renderer::BeginFrame(float r, float g, float b, float a) {
    if (!m_initialized) {
        return;
    }

    const float clearColor[4] = {r, g, b, a};
    m_impl->context->OMSetRenderTargets(1, &m_impl->sceneRTV, m_impl->depthStencilView);
    m_impl->context->ClearRenderTargetView(m_impl->sceneRTV, clearColor);
    m_impl->context->ClearDepthStencilView(m_impl->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_impl->context->RSSetState(m_impl->rasterizerState);
    m_impl->context->OMSetDepthStencilState(m_impl->depthStencilState, 0);
    m_impl->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void Renderer::Draw(const Mesh& mesh, const Transform& transform, const Camera& camera, const Vec3& tint) {
    if (!m_initialized || !mesh.IsValid()) {
        return;
    }

    const Mat4 mvp = camera.ViewProjectionMatrix() * transform.LocalMatrix();
    m_impl->shader.Bind(m_impl->context);
    m_impl->shader.SetConstants(m_impl->context, mvp, tint);
    mesh.Draw(m_impl->context);
}

void Renderer::DrawWorldTriangles(
    const Vertex* vertices,
    unsigned int vertexCount,
    const Mat4& viewProjection,
    bool depthTest) {
    if (!m_initialized || vertices == nullptr || vertexCount == 0) {
        return;
    }

    const UINT byteWidth = vertexCount * static_cast<UINT>(sizeof(Vertex));
    if (m_impl->dynamicVertexBuffer == nullptr || m_impl->dynamicVertexCapacity < vertexCount) {
        if (m_impl->dynamicVertexBuffer != nullptr) {
            m_impl->dynamicVertexBuffer->Release();
            m_impl->dynamicVertexBuffer = nullptr;
        }

        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.ByteWidth = byteWidth;
        vbDesc.Usage = D3D11_USAGE_DYNAMIC;
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(m_impl->device->CreateBuffer(&vbDesc, nullptr, &m_impl->dynamicVertexBuffer))) {
            return;
        }
        m_impl->dynamicVertexCapacity = vertexCount;
    }

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (FAILED(m_impl->context->Map(m_impl->dynamicVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        return;
    }
    memcpy(mapped.pData, vertices, byteWidth);
    m_impl->context->Unmap(m_impl->dynamicVertexBuffer, 0);

    m_impl->context->RSSetState(m_impl->particleRasterizerState);
    m_impl->context->OMSetDepthStencilState(depthTest ? m_impl->particleDepthState : nullptr, 0);
    m_impl->context->OMSetBlendState(m_impl->alphaBlendState, nullptr, 0xffffffff);

    m_impl->shader.Bind(m_impl->context);
    m_impl->shader.SetConstants(m_impl->context, viewProjection, Vec3::One());

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_impl->context->IASetVertexBuffers(0, 1, &m_impl->dynamicVertexBuffer, &stride, &offset);
    m_impl->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_impl->context->Draw(vertexCount, 0);

    m_impl->context->RSSetState(m_impl->rasterizerState);
    m_impl->context->OMSetDepthStencilState(m_impl->depthStencilState, 0);
    m_impl->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void Renderer::ApplyPostProcess() {
    if (!m_initialized) {
        return;
    }

    // Unbind scene as RT before sampling it.
    ID3D11RenderTargetView* nullRTV = nullptr;
    m_impl->context->OMSetRenderTargets(1, &nullRTV, nullptr);

    m_impl->context->OMSetRenderTargets(1, &m_impl->backBufferRTV, nullptr);

    struct PostConstants {
        float saturation;
        float brightness;
        float contrast;
        float pad;
    };

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (SUCCEEDED(m_impl->context->Map(m_impl->postConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        auto* constants = static_cast<PostConstants*>(mapped.pData);
        constants->saturation = m_impl->postSettings.saturation;
        constants->brightness = m_impl->postSettings.brightness;
        constants->contrast = m_impl->postSettings.contrast;
        constants->pad = 0.0f;
        m_impl->context->Unmap(m_impl->postConstantBuffer, 0);
    }

    m_impl->context->IASetInputLayout(nullptr);
    m_impl->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_impl->context->VSSetShader(m_impl->postVS, nullptr, 0);
    m_impl->context->PSSetShader(m_impl->postPS, nullptr, 0);
    m_impl->context->VSSetConstantBuffers(0, 1, &m_impl->postConstantBuffer);
    m_impl->context->PSSetConstantBuffers(0, 1, &m_impl->postConstantBuffer);
    m_impl->context->PSSetShaderResources(0, 1, &m_impl->sceneSRV);
    m_impl->context->PSSetSamplers(0, 1, &m_impl->postSampler);
    m_impl->context->RSSetState(m_impl->particleRasterizerState);
    m_impl->context->OMSetDepthStencilState(nullptr, 0);
    m_impl->context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

    m_impl->context->Draw(3, 0);

    ID3D11ShaderResourceView* nullSRV = nullptr;
    m_impl->context->PSSetShaderResources(0, 1, &nullSRV);

    // Keep backbuffer bound for UI pass.
    m_impl->context->OMSetRenderTargets(1, &m_impl->backBufferRTV, nullptr);
}

void Renderer::EndFrame() {
    if (!m_initialized) {
        return;
    }
    m_impl->swapChain->Present(1, 0);
}

void Renderer::SetPostProcessSettings(const PostProcessSettings& settings) {
    m_impl->postSettings = settings;
}

PostProcessSettings Renderer::GetPostProcessSettings() const {
    return m_impl->postSettings;
}

ID3D11Device* Renderer::GetDevice() const {
    return m_impl != nullptr ? m_impl->device : nullptr;
}

float Renderer::AspectRatio() const {
    if (m_impl == nullptr || m_impl->height == 0) {
        return 16.0f / 9.0f;
    }
    return static_cast<float>(m_impl->width) / static_cast<float>(m_impl->height);
}

bool Renderer::CreateDeviceAndSwapChain(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 1;
    swapDesc.BufferDesc.Width = static_cast<UINT>(m_impl->width);
    swapDesc.BufferDesc.Height = static_cast<UINT>(m_impl->height);
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = hwnd;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;
    swapDesc.Windowed = TRUE;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT flags = 0;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapDesc,
        &m_impl->swapChain,
        &m_impl->device,
        &featureLevel,
        &m_impl->context);

    if (FAILED(hr) && (flags & D3D11_CREATE_DEVICE_DEBUG) != 0) {
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &swapDesc,
            &m_impl->swapChain,
            &m_impl->device,
            &featureLevel,
            &m_impl->context);
    }

    if (FAILED(hr)) {
        return false;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(m_impl->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)))) {
        return false;
    }

    const HRESULT rtvHr = m_impl->device->CreateRenderTargetView(backBuffer, nullptr, &m_impl->backBufferRTV);
    backBuffer->Release();
    if (FAILED(rtvHr)) {
        return false;
    }

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_impl->width);
    viewport.Height = static_cast<float>(m_impl->height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_impl->context->RSSetViewports(1, &viewport);
    return true;
}

bool Renderer::CreateSceneTargets() {
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = static_cast<UINT>(m_impl->width);
    texDesc.Height = static_cast<UINT>(m_impl->height);
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    if (FAILED(m_impl->device->CreateTexture2D(&texDesc, nullptr, &m_impl->sceneTexture))) {
        return false;
    }
    if (FAILED(m_impl->device->CreateRenderTargetView(m_impl->sceneTexture, nullptr, &m_impl->sceneRTV))) {
        return false;
    }
    if (FAILED(m_impl->device->CreateShaderResourceView(m_impl->sceneTexture, nullptr, &m_impl->sceneSRV))) {
        return false;
    }
    return true;
}

bool Renderer::CreateDepthStencil() {
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = static_cast<UINT>(m_impl->width);
    depthDesc.Height = static_cast<UINT>(m_impl->height);
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    if (FAILED(m_impl->device->CreateTexture2D(&depthDesc, nullptr, &m_impl->depthStencilBuffer))) {
        return false;
    }

    return SUCCEEDED(m_impl->device->CreateDepthStencilView(m_impl->depthStencilBuffer, nullptr, &m_impl->depthStencilView));
}

bool Renderer::CreatePipelineState() {
    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthClipEnable = TRUE;

    if (FAILED(m_impl->device->CreateRasterizerState(&rsDesc, &m_impl->rasterizerState))) {
        return false;
    }

    D3D11_RASTERIZER_DESC particleRs = rsDesc;
    particleRs.CullMode = D3D11_CULL_NONE;
    if (FAILED(m_impl->device->CreateRasterizerState(&particleRs, &m_impl->particleRasterizerState))) {
        return false;
    }

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    if (FAILED(m_impl->device->CreateDepthStencilState(&dsDesc, &m_impl->depthStencilState))) {
        return false;
    }

    D3D11_DEPTH_STENCIL_DESC particleDs = dsDesc;
    particleDs.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    if (FAILED(m_impl->device->CreateDepthStencilState(&particleDs, &m_impl->particleDepthState))) {
        return false;
    }

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    return SUCCEEDED(m_impl->device->CreateBlendState(&blendDesc, &m_impl->alphaBlendState));
}

bool Renderer::MakeSiblingShaderPath(
    const char* basicShaderPath,
    const char* fileName,
    char* outPath,
    size_t outSize) {
    if (basicShaderPath == nullptr || fileName == nullptr || outPath == nullptr || outSize == 0) {
        return false;
    }

    std::string path(basicShaderPath);
    const size_t slash = path.find_last_of("\\/");
    if (slash == std::string::npos) {
        path = fileName;
    } else {
        path = path.substr(0, slash + 1) + fileName;
    }

    if (path.size() + 1 > outSize) {
        return false;
    }
    memcpy(outPath, path.c_str(), path.size() + 1);
    return true;
}

bool Renderer::CreatePostProcessResources(const char* basicShaderPath) {
    char postPath[MAX_PATH] = {};
    if (!MakeSiblingShaderPath(basicShaderPath, "PostProcess.hlsl", postPath, MAX_PATH)) {
        return false;
    }

    const int wideSize = MultiByteToWideChar(CP_UTF8, 0, postPath, -1, nullptr, 0);
    std::wstring widePath(static_cast<size_t>(wideSize), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, postPath, -1, widePath.data(), wideSize);
    if (!widePath.empty() && widePath.back() == L'\0') {
        widePath.pop_back();
    }

    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    if (FAILED(D3DCompileFromFile(widePath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vsBlob, &errorBlob))) {
        if (errorBlob != nullptr) {
            errorBlob->Release();
        }
        return false;
    }
    if (errorBlob != nullptr) {
        errorBlob->Release();
        errorBlob = nullptr;
    }

    if (FAILED(D3DCompileFromFile(widePath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &psBlob, &errorBlob))) {
        if (errorBlob != nullptr) {
            errorBlob->Release();
        }
        vsBlob->Release();
        return false;
    }
    if (errorBlob != nullptr) {
        errorBlob->Release();
    }

    if (FAILED(m_impl->device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_impl->postVS))) {
        vsBlob->Release();
        psBlob->Release();
        return false;
    }
    if (FAILED(m_impl->device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_impl->postPS))) {
        vsBlob->Release();
        psBlob->Release();
        return false;
    }
    vsBlob->Release();
    psBlob->Release();

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = 16;
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(m_impl->device->CreateBuffer(&cbDesc, nullptr, &m_impl->postConstantBuffer))) {
        return false;
    }

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    return SUCCEEDED(m_impl->device->CreateSamplerState(&sampDesc, &m_impl->postSampler));
}

} // namespace Engine
