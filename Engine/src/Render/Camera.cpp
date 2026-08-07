#include "Engine/Render/Camera.hpp"

#include "Engine/Math/MathUtils.hpp"

namespace Engine {

void Camera::LookAt(const Vec3& target, const Vec3& up) {
    transform.LookAt(target, up);
}

Mat4 Camera::ViewMatrix() const {
    const Vec3 eye = transform.position;
    const Vec3 target = eye + transform.Forward();
    return Mat4::LookAt(eye, target, transform.Up());
}

Mat4 Camera::ProjectionMatrix() const {
    return Mat4::Perspective(ToRadians(fovYDegrees), aspect, nearZ, farZ);
}

Mat4 Camera::ViewProjectionMatrix() const {
    return ProjectionMatrix() * ViewMatrix();
}

} // namespace Engine
