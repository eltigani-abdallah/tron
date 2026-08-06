#include "../ECS/Systems/RenderSystem.hpp"
#include <iostream>

void RenderSystem::Draw(EntityRegistry& registry)
{
    
    for (auto& id : registry.GetAllEntities())
    {
        if (!registry.IsAlive(id))
            continue;

        
        Transform* transform = registry.GetComponent<Transform>(id);
        MeshRender* mesh = registry.GetComponent<MeshRender>(id);

        // Si l'entité n'a pas de mesh ou de transform → on ignore
        if (!transform || !mesh)
            continue;

        if (!mesh->visible)
            continue;

        // Debug temporaire (à remplacer par ton moteur Direct3D)
        std::cout << "Rendering entity " << id.index
                  << " at position (" << transform->x << ", "
                  << transform->y << ", " << transform->z << ")"
                  << " using meshId=" << mesh->meshId
                  << " materialId=" << mesh->materialId
                  << std::endl;

        //appeller le moteur de rendu 
        // Ou ton pipeline Direct3D :
        // SetMaterial(mesh->materialId);
        // SetMesh(mesh->meshId);
        // SetTransform(transform);
        // Draw();
    }
}
