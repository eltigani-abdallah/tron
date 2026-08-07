#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Vec3.hpp"

namespace Engine {

class ENGINE_API Transform {
public:
    Vec3 position;
    Quaternion rotation;
    Vec3 scale = Vec3::One();

    Transform() = default;
    Transform(const Vec3& position_, const Quaternion& rotation_, const Vec3& scale_);

    Mat4 LocalMatrix() const;

    Vec3 Forward() const;
    Vec3 Right() const;
    Vec3 Up() const;

    void SetEuler(float pitch, float yaw, float roll);
    void LookAt(const Vec3& target, const Vec3& up = Vec3::Up());
};

} // namespace Engine
