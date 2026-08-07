#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Physics/Collider.hpp"

namespace Engine {

struct Plane {
    Vec3 normal = Vec3::Up();
    float distance = 0.0f;
};

class ENGINE_API Frustum {
public:
    Plane planes[6] = {};

    static Frustum FromViewProjection(const Mat4& viewProjection);

    bool IntersectsAABB(const AABB& box) const;
    bool IntersectsSphere(const Sphere& sphere) const;
};

// World-space AABB used by culling (and reusable for physics bounds).
ENGINE_API AABB ComputeWorldAABB(const Transform& transform, const ColliderComponent& collider);
ENGINE_API AABB ComputeDefaultMeshAABB(const Transform& transform, float halfExtent = 0.5f);

} // namespace Engine
