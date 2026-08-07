#include "Engine/Math/Mat4.hpp"

#include "Engine/Math/Quaternion.hpp"

#include <cmath>

namespace Engine {
namespace {

float& At(Mat4& mat, int row, int col) {
    return mat.m[col * 4 + row];
}

float At(const Mat4& mat, int row, int col) {
    return mat.m[col * 4 + row];
}

} // namespace

Mat4 Mat4::Identity() {
    return Mat4{};
}

Mat4 Mat4::Translation(const Vec3& t) {
    Mat4 result = Identity();
    At(result, 0, 3) = t.x;
    At(result, 1, 3) = t.y;
    At(result, 2, 3) = t.z;
    return result;
}

Mat4 Mat4::Scaling(const Vec3& s) {
    Mat4 result{};
    result.m[0] = s.x;
    result.m[5] = s.y;
    result.m[10] = s.z;
    result.m[15] = 1.0f;
    return result;
}

Mat4 Mat4::Rotation(const Quaternion& q) {
    const Quaternion n = q.Normalized();
    const float xx = n.x * n.x;
    const float yy = n.y * n.y;
    const float zz = n.z * n.z;
    const float xy = n.x * n.y;
    const float xz = n.x * n.z;
    const float yz = n.y * n.z;
    const float wx = n.w * n.x;
    const float wy = n.w * n.y;
    const float wz = n.w * n.z;

    Mat4 result{};
    At(result, 0, 0) = 1.0f - 2.0f * (yy + zz);
    At(result, 1, 0) = 2.0f * (xy + wz);
    At(result, 2, 0) = 2.0f * (xz - wy);
    At(result, 3, 0) = 0.0f;

    At(result, 0, 1) = 2.0f * (xy - wz);
    At(result, 1, 1) = 1.0f - 2.0f * (xx + zz);
    At(result, 2, 1) = 2.0f * (yz + wx);
    At(result, 3, 1) = 0.0f;

    At(result, 0, 2) = 2.0f * (xz + wy);
    At(result, 1, 2) = 2.0f * (yz - wx);
    At(result, 2, 2) = 1.0f - 2.0f * (xx + yy);
    At(result, 3, 2) = 0.0f;

    At(result, 0, 3) = 0.0f;
    At(result, 1, 3) = 0.0f;
    At(result, 2, 3) = 0.0f;
    At(result, 3, 3) = 1.0f;
    return result;
}

Mat4 Mat4::TRS(const Vec3& translation, const Quaternion& rotation, const Vec3& scale) {
    return Translation(translation) * Rotation(rotation) * Scaling(scale);
}

Mat4 Mat4::LookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
    const Vec3 zAxis = (target - eye).Normalized();
    const Vec3 xAxis = Vec3::Cross(up, zAxis).Normalized();
    const Vec3 yAxis = Vec3::Cross(zAxis, xAxis);

    Mat4 result = Identity();
    At(result, 0, 0) = xAxis.x;
    At(result, 0, 1) = xAxis.y;
    At(result, 0, 2) = xAxis.z;
    At(result, 0, 3) = -Vec3::Dot(xAxis, eye);

    At(result, 1, 0) = yAxis.x;
    At(result, 1, 1) = yAxis.y;
    At(result, 1, 2) = yAxis.z;
    At(result, 1, 3) = -Vec3::Dot(yAxis, eye);

    At(result, 2, 0) = zAxis.x;
    At(result, 2, 1) = zAxis.y;
    At(result, 2, 2) = zAxis.z;
    At(result, 2, 3) = -Vec3::Dot(zAxis, eye);
    return result;
}

Mat4 Mat4::Perspective(float fovYRadians, float aspect, float nearZ, float farZ) {
    Mat4 result{};
    const float yScale = 1.0f / std::tan(fovYRadians * 0.5f);
    const float xScale = yScale / aspect;
    const float range = farZ - nearZ;

    At(result, 0, 0) = xScale;
    At(result, 1, 1) = yScale;
    At(result, 2, 2) = farZ / range;
    At(result, 3, 2) = 1.0f;
    At(result, 2, 3) = (-nearZ * farZ) / range;
    At(result, 3, 3) = 0.0f;
    return result;
}

Mat4 Mat4::Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ) {
    Mat4 result = Identity();
    const float rl = right - left;
    const float tb = top - bottom;
    const float fn = farZ - nearZ;

    At(result, 0, 0) = 2.0f / rl;
    At(result, 1, 1) = 2.0f / tb;
    At(result, 2, 2) = 1.0f / fn;
    At(result, 0, 3) = -(right + left) / rl;
    At(result, 1, 3) = -(top + bottom) / tb;
    At(result, 2, 3) = -nearZ / fn;
    return result;
}

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 result{};
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += At(*this, row, k) * At(other, k, col);
            }
            At(result, row, col) = sum;
        }
    }
    return result;
}

Vec3 Mat4::TransformPoint(const Vec3& point) const {
    const float x = At(*this, 0, 0) * point.x + At(*this, 0, 1) * point.y + At(*this, 0, 2) * point.z + At(*this, 0, 3);
    const float y = At(*this, 1, 0) * point.x + At(*this, 1, 1) * point.y + At(*this, 1, 2) * point.z + At(*this, 1, 3);
    const float z = At(*this, 2, 0) * point.x + At(*this, 2, 1) * point.y + At(*this, 2, 2) * point.z + At(*this, 2, 3);
    const float w = At(*this, 3, 0) * point.x + At(*this, 3, 1) * point.y + At(*this, 3, 2) * point.z + At(*this, 3, 3);
    if (std::fabs(w) > 0.000001f) {
        return {x / w, y / w, z / w};
    }
    return {x, y, z};
}

Vec3 Mat4::TransformVector(const Vec3& vector) const {
    return {
        At(*this, 0, 0) * vector.x + At(*this, 0, 1) * vector.y + At(*this, 0, 2) * vector.z,
        At(*this, 1, 0) * vector.x + At(*this, 1, 1) * vector.y + At(*this, 1, 2) * vector.z,
        At(*this, 2, 0) * vector.x + At(*this, 2, 1) * vector.y + At(*this, 2, 2) * vector.z
    };
}

Mat4 Mat4::Transposed() const {
    Mat4 result{};
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            At(result, row, col) = At(*this, col, row);
        }
    }
    return result;
}

const float* Mat4::Data() const {
    return m;
}

} // namespace Engine
