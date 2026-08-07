#include "Engine/Physics/SpatialPartition.hpp"

#include <cmath>
#include <cstdint>
#include <unordered_set>

namespace Engine {
namespace {

std::uint64_t MakePairKey(Entity a, Entity b) {
    const Entity minE = a < b ? a : b;
    const Entity maxE = a < b ? b : a;
    return (static_cast<std::uint64_t>(minE) << 32) | static_cast<std::uint64_t>(maxE);
}

} // namespace

SpatialPartition::SpatialPartition(float cellSize)
    : m_cellSize(cellSize > 0.1f ? cellSize : 2.0f) {
}

void SpatialPartition::Clear() {
    m_cells.clear();
}

void SpatialPartition::Insert(Entity entity, const AABB& bounds) {
    const int minX = static_cast<int>(std::floor(bounds.min.x / m_cellSize));
    const int minY = static_cast<int>(std::floor(bounds.min.y / m_cellSize));
    const int minZ = static_cast<int>(std::floor(bounds.min.z / m_cellSize));
    const int maxX = static_cast<int>(std::floor(bounds.max.x / m_cellSize));
    const int maxY = static_cast<int>(std::floor(bounds.max.y / m_cellSize));
    const int maxZ = static_cast<int>(std::floor(bounds.max.z / m_cellSize));

    for (int z = minZ; z <= maxZ; ++z) {
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                m_cells[CellCoord{x, y, z}].push_back(entity);
            }
        }
    }
}

void SpatialPartition::QueryPotentialPairs(std::vector<std::pair<Entity, Entity>>& outPairs) const {
    outPairs.clear();
    std::unordered_set<std::uint64_t> uniquePairs;

    for (const auto& [coord, entities] : m_cells) {
        (void)coord;
        const std::size_t count = entities.size();
        for (std::size_t i = 0; i < count; ++i) {
            for (std::size_t j = i + 1; j < count; ++j) {
                const Entity a = entities[i];
                const Entity b = entities[j];
                if (a == b) {
                    continue;
                }
                const std::uint64_t key = MakePairKey(a, b);
                if (uniquePairs.insert(key).second) {
                    outPairs.emplace_back(a, b);
                }
            }
        }
    }
}

} // namespace Engine
