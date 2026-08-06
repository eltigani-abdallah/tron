#include <cstdint>
#include <array>
#include <queue>
#include "EntityId.hpp"
#pragma once

const uint32_t MAX_ENTITIES = 5000;

class EntityRegistry{

private:
    uint32_t currentIndex;
    std::queue<uint32_t> availableIndices;
    std::array<bool, MAX_ENTITIES> aliveEntities;
    std::array<uint32_t, MAX_ENTITIES> generations;

public:
    EntityRegistry();
    EntityId CreateEntity();

    void DestroyEntity(EntityId id);
    bool IsAlive(EntityId id) const;
    void Reset();
    


};
