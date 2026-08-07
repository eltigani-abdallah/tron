#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Physics/Collider.hpp"
#include "Engine/Physics/SpatialPartition.hpp"

#include <cstdint>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Engine {

class World;

class ENGINE_API PhysicsSystem {
public:
    explicit PhysicsSystem(float cellSize = 2.0f);

    void Update(World& world);
    void Reset();

    std::uint32_t LastPairTests() const { return m_lastPairTests; }
    std::uint32_t LastBroadphasePairs() const { return m_lastBroadphasePairs; }

private:
    using PairKey = std::uint64_t;

    static PairKey MakePairKey(Entity a, Entity b);
    static AABB ComputeBounds(const Transform& transform, const ColliderComponent& collider);
    static bool Intersect(const Transform& ta, const ColliderComponent& ca,
                          const Transform& tb, const ColliderComponent& cb,
                          CollisionInfo& infoAtoB);

    void ResolvePenetration(World& world, Entity a, Entity b, const CollisionInfo& infoAtoB);

    SpatialPartition m_grid;
    std::unordered_set<PairKey> m_previousPairs;
    std::unordered_set<PairKey> m_currentPairs;
    std::uint32_t m_lastPairTests = 0;
    std::uint32_t m_lastBroadphasePairs = 0;
};

} // namespace Engine
