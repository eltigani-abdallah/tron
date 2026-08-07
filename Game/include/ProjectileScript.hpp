#pragma once

#include "Engine/Core/Script.hpp"
#include "Engine/ECS/Components.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Particles/ParticleSystem.hpp"
#include "Engine/Physics/Collider.hpp"
#include "Engine/Time/Time.hpp"
#include "GameScore.hpp"

class ProjectileScript : public Engine::Script {
public:
    void Configure(const Engine::Vec3& direction, float speed, float lifetimeSeconds) {
        m_direction = direction.LengthSquared() > 0.0f ? direction.Normalized() : Engine::Vec3::Forward();
        m_speed = speed;
        m_lifetime = lifetimeSeconds;
    }

    void Update(float deltaSeconds) override {
        m_lifetime -= deltaSeconds;
        if (m_lifetime <= 0.0f) {
            GetWorld().QueueDestroy(GetEntity());
            return;
        }

        Engine::TransformComponent* transform =
            GetWorld().GetComponent<Engine::TransformComponent>(GetEntity());
        if (transform != nullptr) {
            transform->local.position += m_direction * (m_speed * deltaSeconds);
        }
    }

    void OnCollisionEnter(const Engine::CollisionInfo& collision) override {
        const Engine::TagComponent* otherTag =
            GetWorld().GetComponent<Engine::TagComponent>(collision.other);
        if (otherTag == nullptr) {
            return;
        }

        if (otherTag->tag == Engine::EntityTag::Player ||
            otherTag->tag == Engine::EntityTag::Projectile) {
            return;
        }

        Engine::TransformComponent* transform =
            GetWorld().GetComponent<Engine::TransformComponent>(GetEntity());
        Engine::ParticleEmitterComponent* emitter =
            GetWorld().GetComponent<Engine::ParticleEmitterComponent>(GetEntity());
        if (transform != nullptr && emitter != nullptr) {
            Engine::ParticleSystem::EmitBurst(
                *emitter,
                transform->local.position,
                m_direction * -1.0f,
                14);
        }

        if (otherTag->tag == Engine::EntityTag::Enemy) {
            // EnemyScript applies kill/score on projectile hit.
            GetWorld().QueueDestroy(GetEntity());
            return;
        }

        if (otherTag->tag == Engine::EntityTag::Obstacle) {
            GameSession::AddScore(2);
            GetWorld().QueueDestroy(GetEntity());
        }
    }

private:
    Engine::Vec3 m_direction = Engine::Vec3::Forward();
    float m_speed = 28.0f;
    float m_lifetime = 2.0f;
};
