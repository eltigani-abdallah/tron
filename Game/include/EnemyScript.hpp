#pragma once

#include "Engine/Core/Script.hpp"
#include "Engine/ECS/Components.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Particles/ParticleSystem.hpp"
#include "Engine/Physics/Collider.hpp"
#include "Engine/Time/Time.hpp"
#include "GameScore.hpp"

class EnemyScript : public Engine::Script {
public:
    void Start() override {
        Engine::MeshComponent* mesh = GetWorld().GetComponent<Engine::MeshComponent>(GetEntity());
        if (mesh != nullptr) {
            m_baseTint = mesh->tint;
        }
    }

    void Update(float deltaSeconds) override {
        Engine::TransformComponent* self =
            GetWorld().GetComponent<Engine::TransformComponent>(GetEntity());
        if (self == nullptr) {
            return;
        }

        const Engine::Entity player = GetWorld().FindFirstWithTag(Engine::EntityTag::Player);
        if (player != Engine::kInvalidEntity) {
            if (const Engine::TransformComponent* playerTransform =
                    GetWorld().GetComponent<Engine::TransformComponent>(player)) {
                Engine::Vec3 toPlayer = playerTransform->local.position - self->local.position;
                toPlayer.y = 0.0f;
                if (toPlayer.LengthSquared() > 0.01f) {
                    toPlayer = toPlayer.Normalized();
                    self->local.position += toPlayer * (m_moveSpeed * deltaSeconds);
                    self->local.LookAt(playerTransform->local.position);
                }
            }
        }

        if (m_flashTimer > 0.0f) {
            m_flashTimer -= deltaSeconds;
            if (m_flashTimer <= 0.0f) {
                if (Engine::MeshComponent* mesh =
                        GetWorld().GetComponent<Engine::MeshComponent>(GetEntity())) {
                    mesh->tint = m_baseTint;
                }
            }
        }
    }

    void OnCollisionEnter(const Engine::CollisionInfo& collision) override {
        const Engine::TagComponent* otherTag =
            GetWorld().GetComponent<Engine::TagComponent>(collision.other);
        if (otherTag == nullptr) {
            return;
        }

        if (otherTag->tag == Engine::EntityTag::Projectile) {
            TakeHit();
            return;
        }

        if (otherTag->tag == Engine::EntityTag::Player) {
            GameSession::DamagePlayer(18);
            if (Engine::Time* time = GetWorld().GetTime()) {
                time->TriggerSlowMotion(0.3f, 0.35f);
            }
            Flash();
        }
    }

private:
    void Flash() {
        if (Engine::MeshComponent* mesh = GetWorld().GetComponent<Engine::MeshComponent>(GetEntity())) {
            mesh->tint = {1.0f, 0.35f, 0.15f};
            m_flashTimer = 0.2f;
        }
    }

    void TakeHit() {
        --m_hits;
        Flash();

        Engine::TransformComponent* transform =
            GetWorld().GetComponent<Engine::TransformComponent>(GetEntity());
        Engine::ParticleEmitterComponent* emitter =
            GetWorld().GetComponent<Engine::ParticleEmitterComponent>(GetEntity());
        if (transform != nullptr && emitter != nullptr) {
            Engine::ParticleSystem::EmitBurst(
                *emitter,
                transform->local.position,
                Engine::Vec3::Up(),
                22);
        }

        if (m_hits <= 0) {
            GameSession::AddKill();
            if (Engine::Time* time = GetWorld().GetTime()) {
                time->TriggerSlowMotion(0.35f, 0.4f);
            }
            GetWorld().QueueDestroy(GetEntity());
        }
    }

    int m_hits = 2;
    float m_moveSpeed = 2.8f;
    float m_flashTimer = 0.0f;
    Engine::Vec3 m_baseTint = {1.0f, 0.18f, 0.42f};
};
