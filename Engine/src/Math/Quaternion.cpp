#include "Engine/Math/Quaternion.hpp"

#include <cmath>

namespace Engine {
namespace {

constexpr float kEpsilon = 0.000001f;

} // namespace

Quaternion::Quaternion(float x_, float y_, float z_, float w_)
    : x(x_), y(y_), z(z_), w(w_) {
}

Quaternion Quaternion::Identity() {
    return {0.0f, 0.0f, 0.0f, 1.0f};
}

Quaternion Quaternion::FromAxisAngle(const Vec3& axis, float angleRadians) {
    const Vec3 n = axis.Normalized();
    const float half = angleRadians * 0.5f;
    const float s = std::sin(half);
    return {n.x * s, n.y * s, n.z * s, std::cos(half)};
}

Quaternion Quaternion::FromEuler(float pitch, float yaw, float roll) {
    const float halfPitch = pitch * 0.5f;
    const float halfYaw = yaw * 0.5f;
    const float halfRoll = roll * 0.5f;

    const float cy = std::cos(halfYaw);
    const float sy = std::sin(halfYaw);
    const float cp = std::cos(halfPitch);
    const float sp = std::sin(halfPitch);
    const float cr = std::cos(halfRoll);
    const float sr = std::sin(halfRoll);

    Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    return q.Normalized();
}

Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t) {
    float cosTheta = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    Quaternion end = b;

    if (cosTheta < 0.0f) {
        end = {-b.x, -b.y, -b.z, -b.w};
        cosTheta = -cosTheta;
    }

    if (cosTheta > 0.9995f) {
        Quaternion result = {
            a.x + t * (end.x - a.x),
            a.y + t * (end.y - a.y),
            a.z + t * (end.z - a.z),
            a.w + t * (end.w - a.w)
        };
        return result.Normalized();
    }

    const float theta = std::acos(cosTheta);
    const float sinTheta = std::sin(theta);
    const float w1 = std::sin((1.0f - t) * theta) / sinTheta;
    const float w2 = std::sin(t * theta) / sinTheta;

    return {
        a.x * w1 + end.x * w2,
        a.y * w1 + end.y * w2,
        a.z * w1 + end.z * w2,
        a.w * w1 + end.w * w2
    };
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
    return {
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w,
        w * other.w - x * other.x - y * other.y - z * other.z
    };
}

Quaternion Quaternion::Conjugate() const {
    return {-x, -y, -z, w};
}

float Quaternion::Length() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

Quaternion Quaternion::Normalized() const {
    const float len = Length();
    if (len <= kEpsilon) {
        return Identity();
    }
    return {x / len, y / len, z / len, w / len};
}

void Quaternion::Normalize() {
    *this = Normalized();
}

Vec3 Quaternion::Rotate(const Vec3& v) const {
    const Quaternion p(v.x, v.y, v.z, 0.0f);
    const Quaternion r = (*this) * p * Conjugate();
    return {r.x, r.y, r.z};
}

} // namespace Engine
