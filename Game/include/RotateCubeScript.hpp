#pragma once

#include "Engine/Core/Script.hpp"
#include "Engine/ECS/Components.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Particles/ParticleSystem.hpp"
#include "Engine/Physics/Collider.hpp"

class RotateCubeScript : public Engine::Script {
public:
    void Start() override {
        Engine::TransformComponent* transform =
            GetWorld().GetComponent<Engine::TransformComponent>(GetEntity());
        if (transform != nullptr) {
            transform->local.scale = Engine::Vec3::One();
        }

        Engine::MeshComponent* mesh = GetWorld().GetComponent<Engine::MeshComponent>(GetEntity());
        if (mesh != nullptr) {
            m_baseTint = mesh->tint;
        }
    }

    void Update(float deltaSeconds) override {
        m_yaw += deltaSeconds * Engine::ToRadians(45.0f);

        Engine::TransformComponent* transform =
            GetWorld().GetComponent<Engine::TransformComponent>(GetEntity());
        if (transform != nullptr) {
            transform->local.SetEuler(Engine::ToRadians(20.0f), m_yaw, 0.0f);
        }

        if (m_flashTimer > 0.0f) {
            m_flashTimer -= deltaSeconds;
            if (m_flashTimer <= 0.0f) {
                Engine::MeshComponent* mesh = GetWorld().GetComponent<Engine::MeshComponent>(GetEntity());
                if (mesh != nullptr) {
                    mesh->tint = m_baseTint;
                }
            }
        }
    }

    void OnCollisionEnter(const Engine::CollisionInfo&) override {
        Engine::MeshComponent* mesh = GetWorld().GetComponent<Engine::MeshComponent>(GetEntity());
        if (mesh != nullptr) {
            mesh->tint = {1.0f, 0.25f, 0.2f};
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
                20);
        }
    }

private:
    float m_yaw = 0.0f;
    float m_flashTimer = 0.0f;
    Engine::Vec3 m_baseTint = Engine::Vec3::One();
};
