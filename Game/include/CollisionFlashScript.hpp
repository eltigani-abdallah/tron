#pragma once

#include "Engine/Core/Script.hpp"
#include "Engine/ECS/Components.hpp"
#include "Engine/Particles/ParticleSystem.hpp"
#include "Engine/Physics/Collider.hpp"

class CollisionFlashScript : public Engine::Script {
public:
    void Start() override {
        Engine::MeshComponent* mesh = GetWorld().GetComponent<Engine::MeshComponent>(GetEntity());
        if (mesh != nullptr) {
            m_baseTint = mesh->tint;
        }
    }

    void Update(float deltaSeconds) override {
        if (m_flashTimer <= 0.0f) {
            return;
        }

        m_flashTimer -= deltaSeconds;
        Engine::MeshComponent* mesh = GetWorld().GetComponent<Engine::MeshComponent>(GetEntity());
        if (mesh == nullptr) {
            return;
        }

        if (m_flashTimer <= 0.0f) {
            mesh->tint = m_baseTint;
        }
    }

    void OnCollisionEnter(const Engine::CollisionInfo&) override {
        Engine::MeshComponent* mesh = GetWorld().GetComponent<Engine::MeshComponent>(GetEntity());
        if (mesh != nullptr) {
            mesh->tint = {1.0f, 0.2f, 0.2f};
            m_flashTimer = 0.35f;
        }

        Engine::ParticleEmitterComponent* emitter =
            GetWorld().GetComponent<Engine::ParticleEmitterComponent>(GetEntity());
        Engine::TransformComponent* transform =
            GetWorld().GetComponent<Engine::TransformComponent>(GetEntity());
        if (emitter != nullptr && transform != nullptr) {
            Engine::ParticleSystem::EmitBurst(
                *emitter,
                transform->local.position,
                Engine::Vec3::Up(),
                24);
        }
    }

private:
    Engine::Vec3 m_baseTint = Engine::Vec3::One();
    float m_flashTimer = 0.0f;
};
