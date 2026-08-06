#include "EntityRegistry.hpp"

EntityRegistry::EntityRegistry()
{
    currentIndex = 0;
    aliveEntities.fill(false);
    generations.fill(0);
}

EntityId EntityRegistry::CreateEntity()
{
    uint32_t index;

    if (!availableIndices.empty())
    {
        index = availableIndices.front();
        availableIndices.pop();
    }
    else
    {
        index = currentIndex++;

        if (currentIndex >= MAX_ENTITIES)
        {
            printf("Error: too many entities\n");
        }
    }

    aliveEntities[index] = true;

    EntityId id;
    id.index = index;
    id.generation = generations[index];

    return id;
}

void EntityRegistry::DestroyEntity(EntityId id)
{
    uint32_t index = id.index;

    if (index >= MAX_ENTITIES)
        return;

    aliveEntities[index] = false;
    generations[index]++;
    availableIndices.push(index);
}

bool EntityRegistry::IsAlive(EntityId id) const
{
    uint32_t index = id.index;

    if (index >= MAX_ENTITIES)
        return false;

    return aliveEntities[index] &&
           generations[index] == id.generation;
}

void EntityRegistry::Reset()
{
    currentIndex = 0;

    while (!availableIndices.empty())
        availableIndices.pop();

    aliveEntities.fill(false);
    generations.fill(0);
}
