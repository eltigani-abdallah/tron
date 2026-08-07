#include "Engine/Render/Shader.hpp"

#include "Engine/Render/Vertex.hpp"

#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <string>

namespace Engine {
namespace {

struct Constants {
    float worldViewProj[16];
    float tint[4];
};

void CopyMatrix(float* dst, const Mat4& src) {
    for (int i = 0; i < 16; ++i) {
        dst[i] = src.m[i];
    }
}

std::wstring ToWidePath(const char* path) {
    const int size = MultiByteToWideChar(CP_UTF8, 0, path, -1, nullptr, 0);
    std::wstring result(static_cast<size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, path, -1, result.data(), size);
    if (!result.empty() && result.back() == L'\0') {
        result.pop_back();
    }
    return result;
}

} // namespace

Shader::~Shader() {
    Release();
}

bool Shader::LoadFromFile(ID3D11Device* device, const char* hlslPath) {
    Release();
    if (device == nullptr || hlslPath == nullptr) {
        return false;
    }

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    const std::wstring path = ToWidePath(hlslPath);

    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = D3DCompileFromFile(
        path.c_str(),
        nullptr,
        nullptr,
        "VSMain",
        "vs_5_0",
        compileFlags,
        0,
        &vsBlob,
        &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob != nullptr) {
            errorBlob->Release();
        }
        return false;
    }

    if (errorBlob != nullptr) {
        errorBlob->Release();
        errorBlob = nullptr;
    }

    hr = D3DCompileFromFile(
        path.c_str(),
        nullptr,
        nullptr,
        "PSMain",
        "ps_5_0",
        compileFlags,
        0,
        &psBlob,
        &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob != nullptr) {
            errorBlob->Release();
        }
        vsBlob->Release();
        return false;
    }

    if (errorBlob != nullptr) {
        errorBlob->Release();
    }

    if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader))) {
        vsBlob->Release();
        psBlob->Release();
        Release();
        return false;
    }

    if (FAILED(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader))) {
        vsBlob->Release();
        psBlob->Release();
        Release();
        return false;
    }

    const D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    hr = device->CreateInputLayout(
        layout,
        2,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &m_inputLayout);

    vsBlob->Release();
    psBlob->Release();

    if (FAILED(hr)) {
        Release();
        return false;
    }

    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(Constants);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    if (FAILED(device->CreateBuffer(&cbDesc, nullptr, &m_constantBuffer))) {
        Release();
        return false;
    }

    return true;
}

void Shader::Bind(ID3D11DeviceContext* context) const {
    if (context == nullptr || !IsValid()) {
        return;
    }
    context->IASetInputLayout(m_inputLayout);
    context->VSSetShader(m_vertexShader, nullptr, 0);
    context->PSSetShader(m_pixelShader, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &m_constantBuffer);
    context->PSSetConstantBuffers(0, 1, &m_constantBuffer);
}

void Shader::SetConstants(ID3D11DeviceContext* context, const Mat4& worldViewProj, const Vec3& tintRgb) const {
    if (context == nullptr || m_constantBuffer == nullptr) {
        return;
    }

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (FAILED(context->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        return;
    }

    auto* constants = static_cast<Constants*>(mapped.pData);
    CopyMatrix(constants->worldViewProj, worldViewProj);
    constants->tint[0] = tintRgb.x;
    constants->tint[1] = tintRgb.y;
    constants->tint[2] = tintRgb.z;
    constants->tint[3] = 1.0f;
    context->Unmap(m_constantBuffer, 0);
}

void Shader::Release() {
    if (m_constantBuffer != nullptr) {
        m_constantBuffer->Release();
        m_constantBuffer = nullptr;
    }
    if (m_inputLayout != nullptr) {
        m_inputLayout->Release();
        m_inputLayout = nullptr;
    }
    if (m_pixelShader != nullptr) {
        m_pixelShader->Release();
        m_pixelShader = nullptr;
    }
    if (m_vertexShader != nullptr) {
        m_vertexShader->Release();
        m_vertexShader = nullptr;
    }
}

} // namespace Engine
