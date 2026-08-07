#pragma once

#include "Engine/ECS/Entity.hpp"
#include "Engine/Export.hpp"
#include "Engine/Physics/Collider.hpp"

namespace Engine {

class World;

// Base script component (Unity MonoBehaviour-like).
class ENGINE_API Script {
public:
    virtual ~Script();

    virtual void Start() {}
    virtual void Update(float deltaSeconds) { (void)deltaSeconds; }
    virtual void LateUpdate(float deltaSeconds) { (void)deltaSeconds; }
    virtual void OnCollisionEnter(const CollisionInfo& collision) { (void)collision; }

    Entity GetEntity() const { return m_entity; }
    World& GetWorld() const { return *m_world; }

private:
    friend class ScriptManager;

    Entity m_entity = kInvalidEntity;
    World* m_world = nullptr;
    bool m_started = false;
};

} // namespace Engine
