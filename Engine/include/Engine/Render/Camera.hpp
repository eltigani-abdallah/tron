#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Transform.hpp"

namespace Engine {

class ENGINE_API Camera {
public:
    Transform transform;
    float fovYDegrees = 60.0f;
    float nearZ = 0.1f;
    float farZ = 100.0f;
    float aspect = 16.0f / 9.0f;

    Camera() = default;

    void LookAt(const Vec3& target, const Vec3& up = Vec3::Up());
    Mat4 ViewMatrix() const;
    Mat4 ProjectionMatrix() const;
    Mat4 ViewProjectionMatrix() const;
};

} // namespace Engine
