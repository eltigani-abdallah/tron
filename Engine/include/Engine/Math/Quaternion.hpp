#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Vec3.hpp"

namespace Engine {

class ENGINE_API Quaternion {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    Quaternion() = default;
    Quaternion(float x_, float y_, float z_, float w_);

    static Quaternion Identity();
    static Quaternion FromAxisAngle(const Vec3& axis, float angleRadians);
    static Quaternion FromEuler(float pitch, float yaw, float roll);
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

    Quaternion operator*(const Quaternion& other) const;
    Quaternion Conjugate() const;
    float Length() const;
    Quaternion Normalized() const;
    void Normalize();

    Vec3 Rotate(const Vec3& v) const;
};

} // namespace Engine
