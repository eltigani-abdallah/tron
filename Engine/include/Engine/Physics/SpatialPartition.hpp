#pragma once

#include "Engine/ECS/Entity.hpp"
#include "Engine/Export.hpp"
#include "Engine/Physics/Collider.hpp"

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Engine {

class ENGINE_API SpatialPartition {
public:
    explicit SpatialPartition(float cellSize = 2.0f);

    void Clear();
    void Insert(Entity entity, const AABB& bounds);

    // Builds unique unordered pairs of entities that share at least one cell.
    void QueryPotentialPairs(std::vector<std::pair<Entity, Entity>>& outPairs) const;

    std::uint32_t CellCount() const { return static_cast<std::uint32_t>(m_cells.size()); }
    float CellSize() const { return m_cellSize; }

private:
    struct CellCoord {
        int x = 0;
        int y = 0;
        int z = 0;

        bool operator==(const CellCoord& other) const {
            return x == other.x && y == other.y && z == other.z;
        }
    };

    struct CellCoordHash {
        std::size_t operator()(const CellCoord& c) const {
            const std::size_t hx = static_cast<std::size_t>(c.x) * 73856093u;
            const std::size_t hy = static_cast<std::size_t>(c.y) * 19349663u;
            const std::size_t hz = static_cast<std::size_t>(c.z) * 83492791u;
            return hx ^ hy ^ hz;
        }
    };

    float m_cellSize = 2.0f;
    std::unordered_map<CellCoord, std::vector<Entity>, CellCoordHash> m_cells;
};

} // namespace Engine
