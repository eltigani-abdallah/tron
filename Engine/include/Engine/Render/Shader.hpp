#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Vec3.hpp"

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11VertexShader;

namespace Engine {

class ENGINE_API Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    bool LoadFromFile(ID3D11Device* device, const char* hlslPath);
    void Bind(ID3D11DeviceContext* context) const;
    void SetConstants(ID3D11DeviceContext* context, const Mat4& worldViewProj, const Vec3& tintRgb) const;
    void Release();

    bool IsValid() const { return m_vertexShader != nullptr && m_pixelShader != nullptr; }

private:
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_inputLayout = nullptr;
    ID3D11Buffer* m_constantBuffer = nullptr;
};

} // namespace Engine
