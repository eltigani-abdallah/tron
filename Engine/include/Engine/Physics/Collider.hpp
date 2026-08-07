#pragma once

#include "Engine/ECS/Entity.hpp"
#include "Engine/Math/Vec3.hpp"

namespace Engine {

enum class ColliderShape {
    Sphere,
    AABB
};

struct ColliderComponent {
    ColliderShape shape = ColliderShape::Sphere;
    Vec3 offset = Vec3::Zero();
    // Sphere: radius. AABB: half-extents.
    float radius = 0.5f;
    Vec3 halfExtents = {0.5f, 0.5f, 0.5f};
    bool isTrigger = false;
    bool isStatic = true;
};

struct AABB {
    Vec3 min = Vec3::Zero();
    Vec3 max = Vec3::Zero();
};

struct Sphere {
    Vec3 center = Vec3::Zero();
    float radius = 0.5f;
};

struct CollisionInfo {
    Entity other = kInvalidEntity;
    Vec3 normal = Vec3::Zero();
    float penetration = 0.0f;
};

} // namespace Engine
