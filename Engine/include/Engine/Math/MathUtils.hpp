#pragma once

namespace Engine {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kDegToRad = kPi / 180.0f;
constexpr float kRadToDeg = 180.0f / kPi;

inline float ToRadians(float degrees) {
    return degrees * kDegToRad;
}

inline float ToDegrees(float radians) {
    return radians * kRadToDeg;
}

template <typename T>
inline T Clamp(T value, T minValue, T maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

template <typename T>
inline T Lerp(const T& a, const T& b, float t) {
    return a + (b - a) * t;
}

} // namespace Engine
