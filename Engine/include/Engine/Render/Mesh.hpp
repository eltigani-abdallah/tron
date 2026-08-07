#pragma once

#include "Engine/Export.hpp"
#include "Engine/Render/Vertex.hpp"

#include <cstdint>
#include <vector>

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Engine {

class ENGINE_API Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    bool Create(ID3D11Device* device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    static Mesh CreateCube(ID3D11Device* device, float size = 1.0f);

    void Draw(ID3D11DeviceContext* context) const;
    void Release();

    uint32_t IndexCount() const { return m_indexCount; }
    bool IsValid() const { return m_vertexBuffer != nullptr && m_indexBuffer != nullptr; }

private:
    ID3D11Buffer* m_vertexBuffer = nullptr;
    ID3D11Buffer* m_indexBuffer = nullptr;
    uint32_t m_indexCount = 0;
};

} // namespace Engine
