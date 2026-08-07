#pragma once

#include "Engine/Core/Script.hpp"
#include "Engine/Core/ScriptManager.hpp"
#include "Engine/ECS/Components.hpp"
#include "Engine/ECS/Entity.hpp"
#include "Engine/Export.hpp"
#include "Engine/Particles/ParticleEmitter.hpp"
#include "Engine/Physics/Collider.hpp"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct ID3D11Device;

namespace Engine {

class Camera;
class InputManager;
class Time;

class ENGINE_API World {
public:
    World();
    ~World();

    World(const World&) = delete;
    World& operator=(const World&) = delete;

    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    void QueueDestroy(Entity entity);
    void FlushDestroyed();
    void Clear();
    bool IsAlive(Entity entity) const;

    void SetInput(InputManager* input) { m_input = input; }
    InputManager* GetInput() const { return m_input; }

    void SetCamera(Camera* camera) { m_camera = camera; }
    Camera* GetCamera() const { return m_camera; }

    void SetTime(Time* time) { m_time = time; }
    Time* GetTime() const { return m_time; }

    void SetDevice(ID3D11Device* device) { m_device = device; }
    ID3D11Device* GetDevice() const { return m_device; }

    template <typename T>
    T& AddComponent(Entity entity, T component = T{});

    template <typename T>
    T* GetComponent(Entity entity);

    template <typename T>
    const T* GetComponent(Entity entity) const;

    template <typename T>
    bool HasComponent(Entity entity) const;

    template <typename T>
    const std::unordered_map<Entity, T>& Components() const;

    void AddScript(Entity entity, std::unique_ptr<Script> script);
    Script* GetScript(Entity entity) const;

    void StartScripts();
    void UpdateScripts(float deltaSeconds);
    void LateUpdateScripts(float deltaSeconds);

    const std::vector<Entity>& Entities() const { return m_entities; }
    ScriptManager& Scripts() { return m_scripts; }

    Entity FindFirstWithTag(EntityTag tag) const;

private:
    Entity m_nextEntity = 1;
    std::vector<Entity> m_entities;
    std::unordered_set<Entity> m_alive;
    std::vector<Entity> m_pendingDestroy;
    std::unordered_map<Entity, TransformComponent> m_transforms;
    std::unordered_map<Entity, MeshComponent> m_meshes;
    std::unordered_map<Entity, ColliderComponent> m_colliders;
    std::unordered_map<Entity, ParticleEmitterComponent> m_emitters;
    std::unordered_map<Entity, TagComponent> m_tags;
    ScriptManager m_scripts;
    InputManager* m_input = nullptr;
    Camera* m_camera = nullptr;
    Time* m_time = nullptr;
    ID3D11Device* m_device = nullptr;
};

template <>
inline TransformComponent& World::AddComponent<TransformComponent>(Entity entity, TransformComponent component) {
    auto [it, inserted] = m_transforms.insert_or_assign(entity, std::move(component));
    (void)inserted;
    return it->second;
}

template <>
inline MeshComponent& World::AddComponent<MeshComponent>(Entity entity, MeshComponent component) {
    auto [it, inserted] = m_meshes.insert_or_assign(entity, std::move(component));
    (void)inserted;
    return it->second;
}

template <>
inline ColliderComponent& World::AddComponent<ColliderComponent>(Entity entity, ColliderComponent component) {
    auto [it, inserted] = m_colliders.insert_or_assign(entity, std::move(component));
    (void)inserted;
    return it->second;
}

template <>
inline ParticleEmitterComponent& World::AddComponent<ParticleEmitterComponent>(Entity entity, ParticleEmitterComponent component) {
    auto [it, inserted] = m_emitters.insert_or_assign(entity, std::move(component));
    (void)inserted;
    return it->second;
}

template <>
inline TagComponent& World::AddComponent<TagComponent>(Entity entity, TagComponent component) {
    auto [it, inserted] = m_tags.insert_or_assign(entity, std::move(component));
    (void)inserted;
    return it->second;
}

template <>
inline TransformComponent* World::GetComponent<TransformComponent>(Entity entity) {
    auto it = m_transforms.find(entity);
    return it != m_transforms.end() ? &it->second : nullptr;
}

template <>
inline MeshComponent* World::GetComponent<MeshComponent>(Entity entity) {
    auto it = m_meshes.find(entity);
    return it != m_meshes.end() ? &it->second : nullptr;
}

template <>
inline ColliderComponent* World::GetComponent<ColliderComponent>(Entity entity) {
    auto it = m_colliders.find(entity);
    return it != m_colliders.end() ? &it->second : nullptr;
}

template <>
inline ParticleEmitterComponent* World::GetComponent<ParticleEmitterComponent>(Entity entity) {
    auto it = m_emitters.find(entity);
    return it != m_emitters.end() ? &it->second : nullptr;
}

template <>
inline TagComponent* World::GetComponent<TagComponent>(Entity entity) {
    auto it = m_tags.find(entity);
    return it != m_tags.end() ? &it->second : nullptr;
}

template <>
inline const TransformComponent* World::GetComponent<TransformComponent>(Entity entity) const {
    const auto it = m_transforms.find(entity);
    return it != m_transforms.end() ? &it->second : nullptr;
}

template <>
inline const MeshComponent* World::GetComponent<MeshComponent>(Entity entity) const {
    const auto it = m_meshes.find(entity);
    return it != m_meshes.end() ? &it->second : nullptr;
}

template <>
inline const ColliderComponent* World::GetComponent<ColliderComponent>(Entity entity) const {
    const auto it = m_colliders.find(entity);
    return it != m_colliders.end() ? &it->second : nullptr;
}

template <>
inline const ParticleEmitterComponent* World::GetComponent<ParticleEmitterComponent>(Entity entity) const {
    const auto it = m_emitters.find(entity);
    return it != m_emitters.end() ? &it->second : nullptr;
}

template <>
inline const TagComponent* World::GetComponent<TagComponent>(Entity entity) const {
    const auto it = m_tags.find(entity);
    return it != m_tags.end() ? &it->second : nullptr;
}

template <>
inline bool World::HasComponent<TransformComponent>(Entity entity) const {
    return m_transforms.find(entity) != m_transforms.end();
}

template <>
inline bool World::HasComponent<MeshComponent>(Entity entity) const {
    return m_meshes.find(entity) != m_meshes.end();
}

template <>
inline bool World::HasComponent<ColliderComponent>(Entity entity) const {
    return m_colliders.find(entity) != m_colliders.end();
}

template <>
inline bool World::HasComponent<ParticleEmitterComponent>(Entity entity) const {
    return m_emitters.find(entity) != m_emitters.end();
}

template <>
inline bool World::HasComponent<TagComponent>(Entity entity) const {
    return m_tags.find(entity) != m_tags.end();
}

template <>
inline const std::unordered_map<Entity, TransformComponent>& World::Components<TransformComponent>() const {
    return m_transforms;
}

template <>
inline const std::unordered_map<Entity, MeshComponent>& World::Components<MeshComponent>() const {
    return m_meshes;
}

template <>
inline const std::unordered_map<Entity, ColliderComponent>& World::Components<ColliderComponent>() const {
    return m_colliders;
}

template <>
inline const std::unordered_map<Entity, ParticleEmitterComponent>& World::Components<ParticleEmitterComponent>() const {
    return m_emitters;
}

template <>
inline const std::unordered_map<Entity, TagComponent>& World::Components<TagComponent>() const {
    return m_tags;
}

} // namespace Engine
