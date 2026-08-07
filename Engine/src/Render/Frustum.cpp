#include "Engine/Render/Frustum.hpp"

#include <cmath>

namespace Engine {
namespace {

float At(const Mat4& mat, int row, int col) {
    return mat.m[col * 4 + row];
}

Plane NormalizePlane(Plane plane) {
    const float length = plane.normal.Length();
    if (length > 0.000001f) {
        plane.normal = plane.normal / length;
        plane.distance /= length;
    }
    return plane;
}

float SignedDistance(const Plane& plane, const Vec3& point) {
    return Vec3::Dot(plane.normal, point) + plane.distance;
}

Vec3 WorldCenter(const Transform& transform, const ColliderComponent& collider) {
    return transform.position + transform.rotation.Rotate(collider.offset);
}

} // namespace

Frustum Frustum::FromViewProjection(const Mat4& viewProjection) {
    Frustum frustum;

    const Vec3 row0 = {At(viewProjection, 0, 0), At(viewProjection, 0, 1), At(viewProjection, 0, 2)};
    const float d0 = At(viewProjection, 0, 3);
    const Vec3 row1 = {At(viewProjection, 1, 0), At(viewProjection, 1, 1), At(viewProjection, 1, 2)};
    const float d1 = At(viewProjection, 1, 3);
    const Vec3 row2 = {At(viewProjection, 2, 0), At(viewProjection, 2, 1), At(viewProjection, 2, 2)};
    const float d2 = At(viewProjection, 2, 3);
    const Vec3 row3 = {At(viewProjection, 3, 0), At(viewProjection, 3, 1), At(viewProjection, 3, 2)};
    const float d3 = At(viewProjection, 3, 3);

    frustum.planes[0] = NormalizePlane(Plane{row3 + row0, d3 + d0});
    frustum.planes[1] = NormalizePlane(Plane{row3 - row0, d3 - d0});
    frustum.planes[2] = NormalizePlane(Plane{row3 + row1, d3 + d1});
    frustum.planes[3] = NormalizePlane(Plane{row3 - row1, d3 - d1});
    frustum.planes[4] = NormalizePlane(Plane{row3 + row2, d3 + d2});
    frustum.planes[5] = NormalizePlane(Plane{row3 - row2, d3 - d2});

    return frustum;
}

bool Frustum::IntersectsAABB(const AABB& box) const {
    for (const Plane& plane : planes) {
        const Vec3 p{
            plane.normal.x >= 0.0f ? box.max.x : box.min.x,
            plane.normal.y >= 0.0f ? box.max.y : box.min.y,
            plane.normal.z >= 0.0f ? box.max.z : box.min.z
        };

        if (SignedDistance(plane, p) < 0.0f) {
            return false;
        }
    }
    return true;
}

bool Frustum::IntersectsSphere(const Sphere& sphere) const {
    for (const Plane& plane : planes) {
        if (SignedDistance(plane, sphere.center) < -sphere.radius) {
            return false;
        }
    }
    return true;
}

AABB ComputeWorldAABB(const Transform& transform, const ColliderComponent& collider) {
    const Vec3 center = WorldCenter(transform, collider);
    if (collider.shape == ColliderShape::Sphere) {
        const Vec3 r = {collider.radius, collider.radius, collider.radius};
        return {center - r, center + r};
    }

    const Vec3 axes[3] = {
        transform.rotation.Rotate({collider.halfExtents.x, 0.0f, 0.0f}),
        transform.rotation.Rotate({0.0f, collider.halfExtents.y, 0.0f}),
        transform.rotation.Rotate({0.0f, 0.0f, collider.halfExtents.z})
    };
    const Vec3 extent{
        std::abs(axes[0].x) + std::abs(axes[1].x) + std::abs(axes[2].x),
        std::abs(axes[0].y) + std::abs(axes[1].y) + std::abs(axes[2].y),
        std::abs(axes[0].z) + std::abs(axes[1].z) + std::abs(axes[2].z)
    };
    return {center - extent, center + extent};
}

AABB ComputeDefaultMeshAABB(const Transform& transform, float halfExtent) {
    const Vec3 scaled{
        halfExtent * transform.scale.x,
        halfExtent * transform.scale.y,
        halfExtent * transform.scale.z
    };
    ColliderComponent proxy;
    proxy.shape = ColliderShape::AABB;
    proxy.halfExtents = scaled;
    return ComputeWorldAABB(transform, proxy);
}

} // namespace Engine
