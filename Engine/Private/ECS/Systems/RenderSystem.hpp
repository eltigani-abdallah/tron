#pragma once
#include "../ECS/EntityRegistry.hpp"
#include "../ECS/Components/Transform.hpp"
#include "../ECS/Components/MeshRender.hpp"

class RenderSystem
{
public:
    RenderSystem() = default;

    void Draw(EntityRegistry& registry);
};
