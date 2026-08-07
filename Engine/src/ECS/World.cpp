#include "Engine/ECS/World.hpp"

namespace Engine {

World::World() = default;

World::~World() {
    Clear();
}

Entity World::CreateEntity() {
    const Entity entity = m_nextEntity++;
    m_entities.push_back(entity);
    m_alive.insert(entity);
    return entity;
}

void World::DestroyEntity(Entity entity) {
    if (!IsAlive(entity)) {
        return;
    }

    m_scripts.Detach(entity);
    m_transforms.erase(entity);
    m_meshes.erase(entity);
    m_colliders.erase(entity);
    m_emitters.erase(entity);
    m_tags.erase(entity);
    m_alive.erase(entity);

    for (auto it = m_entities.begin(); it != m_entities.end(); ++it) {
        if (*it == entity) {
            m_entities.erase(it);
            break;
        }
    }
}

void World::QueueDestroy(Entity entity) {
    if (!IsAlive(entity)) {
        return;
    }
    m_pendingDestroy.push_back(entity);
}

void World::FlushDestroyed() {
    if (m_pendingDestroy.empty()) {
        return;
    }

    // Unique pending ids.
    std::unordered_set<Entity> unique(m_pendingDestroy.begin(), m_pendingDestroy.end());
    m_pendingDestroy.clear();
    for (Entity entity : unique) {
        DestroyEntity(entity);
    }
}

void World::Clear() {
    m_scripts.Clear();
    m_transforms.clear();
    m_meshes.clear();
    m_colliders.clear();
    m_emitters.clear();
    m_tags.clear();
    m_entities.clear();
    m_alive.clear();
    m_pendingDestroy.clear();
    m_nextEntity = 1;
    // Keep service pointers wired by Application.
}

bool World::IsAlive(Entity entity) const {
    return m_alive.find(entity) != m_alive.end();
}

void World::AddScript(Entity entity, std::unique_ptr<Script> script) {
    m_scripts.Attach(entity, std::move(script), this);
}

Script* World::GetScript(Entity entity) const {
    return m_scripts.Get(entity);
}

void World::StartScripts() {
    m_scripts.StartAll();
}

void World::UpdateScripts(float deltaSeconds) {
    m_scripts.UpdateAll(deltaSeconds);
}

void World::LateUpdateScripts(float deltaSeconds) {
    m_scripts.LateUpdateAll(deltaSeconds);
}

Entity World::FindFirstWithTag(EntityTag tag) const {
    for (const auto& [entity, tagComponent] : m_tags) {
        if (tagComponent.tag == tag && IsAlive(entity)) {
            return entity;
        }
    }
    return kInvalidEntity;
}

} // namespace Engine
