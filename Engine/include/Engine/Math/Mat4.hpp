#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Vec3.hpp"

namespace Engine {

class Quaternion;

// Column-major 4x4 matrix (Direct3D / HLSL friendly).
class ENGINE_API Mat4 {
public:
    float m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    Mat4() = default;

    static Mat4 Identity();
    static Mat4 Translation(const Vec3& t);
    static Mat4 Scaling(const Vec3& s);
    static Mat4 Rotation(const Quaternion& q);
    static Mat4 TRS(const Vec3& translation, const Quaternion& rotation, const Vec3& scale);

    static Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up);
    static Mat4 Perspective(float fovYRadians, float aspect, float nearZ, float farZ);
    static Mat4 Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ);

    Mat4 operator*(const Mat4& other) const;
    Vec3 TransformPoint(const Vec3& point) const;
    Vec3 TransformVector(const Vec3& vector) const;

    Mat4 Transposed() const;
    const float* Data() const;
};

} // namespace Engine
