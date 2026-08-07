#include "Engine/Math/Transform.hpp"

#include <cmath>

namespace Engine {

Transform::Transform(const Vec3& position_, const Quaternion& rotation_, const Vec3& scale_)
    : position(position_), rotation(rotation_), scale(scale_) {
}

Mat4 Transform::LocalMatrix() const {
    return Mat4::TRS(position, rotation, scale);
}

Vec3 Transform::Forward() const {
    return rotation.Rotate(Vec3::Forward());
}

Vec3 Transform::Right() const {
    return rotation.Rotate(Vec3::Right());
}

Vec3 Transform::Up() const {
    return rotation.Rotate(Vec3::Up());
}

void Transform::SetEuler(float pitch, float yaw, float roll) {
    rotation = Quaternion::FromEuler(pitch, yaw, roll);
}

void Transform::LookAt(const Vec3& target, const Vec3& up) {
    const Vec3 forward = (target - position).Normalized();
    if (forward.LengthSquared() <= 0.000001f) {
        return;
    }

    const Vec3 right = Vec3::Cross(up, forward).Normalized();
    const Vec3 correctedUp = Vec3::Cross(forward, right);

    // Build rotation matrix columns (right, up, forward) then convert to quaternion.
    const float m00 = right.x;
    const float m01 = correctedUp.x;
    const float m02 = forward.x;
    const float m10 = right.y;
    const float m11 = correctedUp.y;
    const float m12 = forward.y;
    const float m20 = right.z;
    const float m21 = correctedUp.z;
    const float m22 = forward.z;

    Quaternion q;
    const float trace = m00 + m11 + m22;
    if (trace > 0.0f) {
        const float s = std::sqrt(trace + 1.0f) * 2.0f;
        q.w = 0.25f * s;
        q.x = (m21 - m12) / s;
        q.y = (m02 - m20) / s;
        q.z = (m10 - m01) / s;
    } else if (m00 > m11 && m00 > m22) {
        const float s = std::sqrt(1.0f + m00 - m11 - m22) * 2.0f;
        q.w = (m21 - m12) / s;
        q.x = 0.25f * s;
        q.y = (m01 + m10) / s;
        q.z = (m02 + m20) / s;
    } else if (m11 > m22) {
        const float s = std::sqrt(1.0f + m11 - m00 - m22) * 2.0f;
        q.w = (m02 - m20) / s;
        q.x = (m01 + m10) / s;
        q.y = 0.25f * s;
        q.z = (m12 + m21) / s;
    } else {
        const float s = std::sqrt(1.0f + m22 - m00 - m11) * 2.0f;
        q.w = (m10 - m01) / s;
        q.x = (m02 + m20) / s;
        q.y = (m12 + m21) / s;
        q.z = 0.25f * s;
    }

    rotation = q.Normalized();
}

} // namespace Engine
