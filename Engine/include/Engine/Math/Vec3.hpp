#pragma once

#include "Engine/Export.hpp"

namespace Engine {

class ENGINE_API Vec3 {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float x_, float y_, float z_);

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    Vec3 operator/(float scalar) const;

    Vec3& operator+=(const Vec3& other);
    Vec3& operator-=(const Vec3& other);
    Vec3& operator*=(float scalar);

    Vec3 operator-() const;

    float Length() const;
    float LengthSquared() const;
    Vec3 Normalized() const;
    void Normalize();

    static float Dot(const Vec3& a, const Vec3& b);
    static Vec3 Cross(const Vec3& a, const Vec3& b);
    static Vec3 Lerp(const Vec3& a, const Vec3& b, float t);

    static Vec3 Zero();
    static Vec3 One();
    static Vec3 Up();
    static Vec3 Forward();
    static Vec3 Right();
};

ENGINE_API Vec3 operator*(float scalar, const Vec3& v);

} // namespace Engine
