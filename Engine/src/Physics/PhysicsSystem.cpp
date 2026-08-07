#include "Engine/Physics/PhysicsSystem.hpp"

#include "Engine/Core/Script.hpp"
#include "Engine/ECS/Components.hpp"
#include "Engine/ECS/World.hpp"
#include "Engine/Math/Transform.hpp"

#include <algorithm>
#include <cmath>

namespace Engine {
namespace {

Vec3 WorldCenter(const Transform& transform, const ColliderComponent& collider) {
    return transform.position + transform.rotation.Rotate(collider.offset);
}

AABB SphereToAABB(const Sphere& sphere) {
    const Vec3 r = {sphere.radius, sphere.radius, sphere.radius};
    return {sphere.center - r, sphere.center + r};
}

bool SphereSphere(const Sphere& a, const Sphere& b, CollisionInfo& infoAtoB) {
    const Vec3 delta = b.center - a.center;
    const float distSq = delta.LengthSquared();
    const float radiusSum = a.radius + b.radius;
    if (distSq > radiusSum * radiusSum) {
        return false;
    }

    const float dist = std::sqrt(std::max(distSq, 0.000001f));
    infoAtoB.normal = dist > 0.000001f ? (delta / dist) : Vec3::Up();
    infoAtoB.penetration = radiusSum - dist;
    return true;
}

bool AabbAabb(const AABB& a, const AABB& b, CollisionInfo& infoAtoB) {
    const float dx1 = a.max.x - b.min.x;
    const float dx2 = b.max.x - a.min.x;
    const float dy1 = a.max.y - b.min.y;
    const float dy2 = b.max.y - a.min.y;
    const float dz1 = a.max.z - b.min.z;
    const float dz2 = b.max.z - a.min.z;

    if (dx1 <= 0.0f || dx2 <= 0.0f || dy1 <= 0.0f || dy2 <= 0.0f || dz1 <= 0.0f || dz2 <= 0.0f) {
        return false;
    }

    const float overlapX = std::min(dx1, dx2);
    const float overlapY = std::min(dy1, dy2);
    const float overlapZ = std::min(dz1, dz2);

    if (overlapX <= overlapY && overlapX <= overlapZ) {
        infoAtoB.penetration = overlapX;
        infoAtoB.normal = (a.min.x + a.max.x < b.min.x + b.max.x) ? Vec3{-1.0f, 0.0f, 0.0f} : Vec3{1.0f, 0.0f, 0.0f};
    } else if (overlapY <= overlapZ) {
        infoAtoB.penetration = overlapY;
        infoAtoB.normal = (a.min.y + a.max.y < b.min.y + b.max.y) ? Vec3{0.0f, -1.0f, 0.0f} : Vec3{0.0f, 1.0f, 0.0f};
    } else {
        infoAtoB.penetration = overlapZ;
        infoAtoB.normal = (a.min.z + a.max.z < b.min.z + b.max.z) ? Vec3{0.0f, 0.0f, -1.0f} : Vec3{0.0f, 0.0f, 1.0f};
    }
    return true;
}

bool SphereAabb(const Sphere& sphere, const AABB& box, CollisionInfo& infoSphereToBox) {
    const Vec3 closest{
        std::max(box.min.x, std::min(sphere.center.x, box.max.x)),
        std::max(box.min.y, std::min(sphere.center.y, box.max.y)),
        std::max(box.min.z, std::min(sphere.center.z, box.max.z))
    };

    const Vec3 delta = sphere.center - closest;
    const float distSq = delta.LengthSquared();
    if (distSq > sphere.radius * sphere.radius) {
        return false;
    }

    if (distSq > 0.000001f) {
        const float dist = std::sqrt(distSq);
        infoSphereToBox.normal = delta / dist;
        infoSphereToBox.penetration = sphere.radius - dist;
    } else {
        // Center inside AABB: push out along smallest axis.
        const float dx = std::min(sphere.center.x - box.min.x, box.max.x - sphere.center.x);
        const float dy = std::min(sphere.center.y - box.min.y, box.max.y - sphere.center.y);
        const float dz = std::min(sphere.center.z - box.min.z, box.max.z - sphere.center.z);
        if (dx <= dy && dx <= dz) {
            infoSphereToBox.normal = (sphere.center.x < (box.min.x + box.max.x) * 0.5f)
                ? Vec3{-1.0f, 0.0f, 0.0f}
                : Vec3{1.0f, 0.0f, 0.0f};
            infoSphereToBox.penetration = sphere.radius + dx;
        } else if (dy <= dz) {
            infoSphereToBox.normal = (sphere.center.y < (box.min.y + box.max.y) * 0.5f)
                ? Vec3{0.0f, -1.0f, 0.0f}
                : Vec3{0.0f, 1.0f, 0.0f};
            infoSphereToBox.penetration = sphere.radius + dy;
        } else {
            infoSphereToBox.normal = (sphere.center.z < (box.min.z + box.max.z) * 0.5f)
                ? Vec3{0.0f, 0.0f, -1.0f}
                : Vec3{0.0f, 0.0f, 1.0f};
            infoSphereToBox.penetration = sphere.radius + dz;
        }
    }
    return true;
}

} // namespace

PhysicsSystem::PhysicsSystem(float cellSize)
    : m_grid(cellSize) {
}

void PhysicsSystem::Reset() {
    m_previousPairs.clear();
    m_currentPairs.clear();
    m_grid.Clear();
    m_lastPairTests = 0;
    m_lastBroadphasePairs = 0;
}

PhysicsSystem::PairKey PhysicsSystem::MakePairKey(Entity a, Entity b) {
    const Entity minE = a < b ? a : b;
    const Entity maxE = a < b ? b : a;
    return (static_cast<PairKey>(minE) << 32) | static_cast<PairKey>(maxE);
}

AABB PhysicsSystem::ComputeBounds(const Transform& transform, const ColliderComponent& collider) {
    const Vec3 center = WorldCenter(transform, collider);
    if (collider.shape == ColliderShape::Sphere) {
        return SphereToAABB(Sphere{center, collider.radius});
    }

    // Axis-aligned bounds from rotated half-extents (AABB envelope).
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

bool PhysicsSystem::Intersect(const Transform& ta, const ColliderComponent& ca,
                              const Transform& tb, const ColliderComponent& cb,
                              CollisionInfo& infoAtoB) {
    const Vec3 centerA = WorldCenter(ta, ca);
    const Vec3 centerB = WorldCenter(tb, cb);

    if (ca.shape == ColliderShape::Sphere && cb.shape == ColliderShape::Sphere) {
        return SphereSphere(Sphere{centerA, ca.radius}, Sphere{centerB, cb.radius}, infoAtoB);
    }

    if (ca.shape == ColliderShape::AABB && cb.shape == ColliderShape::AABB) {
        const AABB a = ComputeBounds(ta, ca);
        const AABB b = ComputeBounds(tb, cb);
        return AabbAabb(a, b, infoAtoB);
    }

    if (ca.shape == ColliderShape::Sphere && cb.shape == ColliderShape::AABB) {
        const AABB b = ComputeBounds(tb, cb);
        return SphereAabb(Sphere{centerA, ca.radius}, b, infoAtoB);
    }

    if (ca.shape == ColliderShape::AABB && cb.shape == ColliderShape::Sphere) {
        const AABB a = ComputeBounds(ta, ca);
        CollisionInfo sphereToAabb;
        if (!SphereAabb(Sphere{centerB, cb.radius}, a, sphereToAabb)) {
            return false;
        }
        infoAtoB.normal = sphereToAabb.normal * -1.0f;
        infoAtoB.penetration = sphereToAabb.penetration;
        return true;
    }

    return false;
}

void PhysicsSystem::ResolvePenetration(World& world, Entity a, Entity b, const CollisionInfo& infoAtoB) {
    ColliderComponent* ca = world.GetComponent<ColliderComponent>(a);
    ColliderComponent* cb = world.GetComponent<ColliderComponent>(b);
    TransformComponent* ta = world.GetComponent<TransformComponent>(a);
    TransformComponent* tb = world.GetComponent<TransformComponent>(b);
    if (ca == nullptr || cb == nullptr || ta == nullptr || tb == nullptr) {
        return;
    }
    if (ca->isTrigger || cb->isTrigger) {
        return;
    }

    const Vec3 separation = infoAtoB.normal * infoAtoB.penetration;
    if (!ca->isStatic && cb->isStatic) {
        ta->local.position -= separation;
    } else if (ca->isStatic && !cb->isStatic) {
        tb->local.position += separation;
    } else if (!ca->isStatic && !cb->isStatic) {
        ta->local.position -= separation * 0.5f;
        tb->local.position += separation * 0.5f;
    }
}

void PhysicsSystem::Update(World& world) {
    m_grid.Clear();
    m_currentPairs.clear();
    m_lastPairTests = 0;
    m_lastBroadphasePairs = 0;

    const auto& colliders = world.Components<ColliderComponent>();
    for (const auto& [entity, collider] : colliders) {
        const TransformComponent* transform = world.GetComponent<TransformComponent>(entity);
        if (transform == nullptr) {
            continue;
        }
        m_grid.Insert(entity, ComputeBounds(transform->local, collider));
    }

    std::vector<std::pair<Entity, Entity>> pairs;
    m_grid.QueryPotentialPairs(pairs);
    m_lastBroadphasePairs = static_cast<std::uint32_t>(pairs.size());

    for (const auto& [entityA, entityB] : pairs) {
        TransformComponent* ta = world.GetComponent<TransformComponent>(entityA);
        TransformComponent* tb = world.GetComponent<TransformComponent>(entityB);
        ColliderComponent* ca = world.GetComponent<ColliderComponent>(entityA);
        ColliderComponent* cb = world.GetComponent<ColliderComponent>(entityB);
        if (ta == nullptr || tb == nullptr || ca == nullptr || cb == nullptr) {
            continue;
        }

        CollisionInfo info;
        ++m_lastPairTests;
        if (!Intersect(ta->local, *ca, tb->local, *cb, info)) {
            continue;
        }

        const PairKey key = MakePairKey(entityA, entityB);
        m_currentPairs.insert(key);

        ResolvePenetration(world, entityA, entityB, info);

        if (m_previousPairs.find(key) == m_previousPairs.end()) {
            info.other = entityB;
            if (Script* scriptA = world.GetScript(entityA)) {
                scriptA->OnCollisionEnter(info);
            }

            CollisionInfo infoB = info;
            infoB.other = entityA;
            infoB.normal = info.normal * -1.0f;
            if (Script* scriptB = world.GetScript(entityB)) {
                scriptB->OnCollisionEnter(infoB);
            }
        }
    }

    m_previousPairs.swap(m_currentPairs);
    m_currentPairs.clear();
}

} // namespace Engine
