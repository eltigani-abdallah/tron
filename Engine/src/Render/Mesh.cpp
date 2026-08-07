#include "Engine/Render/Mesh.hpp"

#include <d3d11.h>

#include <iterator>

namespace Engine {

Mesh::~Mesh() {
    Release();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vertexBuffer(other.m_vertexBuffer)
    , m_indexBuffer(other.m_indexBuffer)
    , m_indexCount(other.m_indexCount) {
    other.m_vertexBuffer = nullptr;
    other.m_indexBuffer = nullptr;
    other.m_indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        Release();
        m_vertexBuffer = other.m_vertexBuffer;
        m_indexBuffer = other.m_indexBuffer;
        m_indexCount = other.m_indexCount;
        other.m_vertexBuffer = nullptr;
        other.m_indexBuffer = nullptr;
        other.m_indexCount = 0;
    }
    return *this;
}

bool Mesh::Create(ID3D11Device* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    Release();
    if (device == nullptr || vertices.empty() || indices.empty()) {
        return false;
    }

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();

    if (FAILED(device->CreateBuffer(&vbDesc, &vbData, &m_vertexBuffer))) {
        Release();
        return false;
    }

    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t));
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();

    if (FAILED(device->CreateBuffer(&ibDesc, &ibData, &m_indexBuffer))) {
        Release();
        return false;
    }

    m_indexCount = static_cast<uint32_t>(indices.size());
    return true;
}

Mesh Mesh::CreateCube(ID3D11Device* device, float size) {
    Mesh mesh;
    const float h = size * 0.5f;

    // Per-face shading baked into vertex colors (cheap TRON volume look).
    const Vertex vertices[] = {
        // +Z (front)
        {-h, -h, +h, 0.18f, 0.72f, 0.95f, 1.0f},
        {+h, -h, +h, 0.18f, 0.72f, 0.95f, 1.0f},
        {+h, +h, +h, 0.55f, 0.95f, 1.0f, 1.0f},
        {-h, +h, +h, 0.55f, 0.95f, 1.0f, 1.0f},
        // -Z
        {+h, -h, -h, 0.08f, 0.38f, 0.62f, 1.0f},
        {-h, -h, -h, 0.08f, 0.38f, 0.62f, 1.0f},
        {-h, +h, -h, 0.22f, 0.58f, 0.82f, 1.0f},
        {+h, +h, -h, 0.22f, 0.58f, 0.82f, 1.0f},
        // +X
        {+h, -h, +h, 0.12f, 0.55f, 0.78f, 1.0f},
        {+h, -h, -h, 0.12f, 0.55f, 0.78f, 1.0f},
        {+h, +h, -h, 0.40f, 0.85f, 1.0f, 1.0f},
        {+h, +h, +h, 0.40f, 0.85f, 1.0f, 1.0f},
        // -X
        {-h, -h, -h, 0.06f, 0.42f, 0.60f, 1.0f},
        {-h, -h, +h, 0.06f, 0.42f, 0.60f, 1.0f},
        {-h, +h, +h, 0.28f, 0.70f, 0.88f, 1.0f},
        {-h, +h, -h, 0.28f, 0.70f, 0.88f, 1.0f},
        // +Y
        {-h, +h, +h, 0.70f, 1.0f, 1.0f, 1.0f},
        {+h, +h, +h, 0.70f, 1.0f, 1.0f, 1.0f},
        {+h, +h, -h, 0.55f, 0.92f, 1.0f, 1.0f},
        {-h, +h, -h, 0.55f, 0.92f, 1.0f, 1.0f},
        // -Y
        {-h, -h, -h, 0.02f, 0.16f, 0.28f, 1.0f},
        {+h, -h, -h, 0.02f, 0.16f, 0.28f, 1.0f},
        {+h, -h, +h, 0.04f, 0.22f, 0.36f, 1.0f},
        {-h, -h, +h, 0.04f, 0.22f, 0.36f, 1.0f},
    };

    const uint32_t indices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    mesh.Create(
        device,
        std::vector<Vertex>(std::begin(vertices), std::end(vertices)),
        std::vector<uint32_t>(std::begin(indices), std::end(indices)));
    return mesh;
}

void Mesh::Draw(ID3D11DeviceContext* context) const {
    if (context == nullptr || !IsValid()) {
        return;
    }

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->DrawIndexed(m_indexCount, 0, 0);
}

void Mesh::Release() {
    if (m_indexBuffer != nullptr) {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }
    if (m_vertexBuffer != nullptr) {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }
    m_indexCount = 0;
}

} // namespace Engine
