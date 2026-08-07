#include "Engine/Math/Vec3.hpp"

#include <cmath>

namespace Engine {

Vec3::Vec3(float x_, float y_, float z_)
    : x(x_), y(y_), z(z_) {
}

Vec3 Vec3::operator+(const Vec3& other) const {
    return {x + other.x, y + other.y, z + other.z};
}

Vec3 Vec3::operator-(const Vec3& other) const {
    return {x - other.x, y - other.y, z - other.z};
}

Vec3 Vec3::operator*(float scalar) const {
    return {x * scalar, y * scalar, z * scalar};
}

Vec3 Vec3::operator/(float scalar) const {
    return {x / scalar, y / scalar, z / scalar};
}

Vec3& Vec3::operator+=(const Vec3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vec3& Vec3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vec3 Vec3::operator-() const {
    return {-x, -y, -z};
}

float Vec3::Length() const {
    return std::sqrt(LengthSquared());
}

float Vec3::LengthSquared() const {
    return x * x + y * y + z * z;
}

Vec3 Vec3::Normalized() const {
    const float len = Length();
    if (len <= 0.000001f) {
        return Zero();
    }
    return *this / len;
}

void Vec3::Normalize() {
    *this = Normalized();
}

float Vec3::Dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 Vec3::Cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vec3 Vec3::Lerp(const Vec3& a, const Vec3& b, float t) {
    return a + (b - a) * t;
}

Vec3 Vec3::Zero() {
    return {0.0f, 0.0f, 0.0f};
}

Vec3 Vec3::One() {
    return {1.0f, 1.0f, 1.0f};
}

Vec3 Vec3::Up() {
    return {0.0f, 1.0f, 0.0f};
}

Vec3 Vec3::Forward() {
    return {0.0f, 0.0f, 1.0f};
}

Vec3 Vec3::Right() {
    return {1.0f, 0.0f, 0.0f};
}

Vec3 operator*(float scalar, const Vec3& v) {
    return v * scalar;
}

} // namespace Engine
