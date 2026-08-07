#pragma once

#include "Engine/Core/Script.hpp"
#include "Engine/ECS/Components.hpp"
#include "Engine/Input/InputManager.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Particles/ParticleSystem.hpp"
#include "Engine/Physics/Collider.hpp"
#include "Engine/Render/Camera.hpp"
#include "Engine/Render/Mesh.hpp"
#include "Engine/Time/Time.hpp"
#include "GameScore.hpp"
#include "ProjectileScript.hpp"

#include <d3d11.h>
#include <memory>

class ShipControllerScript : public Engine::Script {
public:
    void Start() override {
        Engine::TransformComponent* transform =
            GetWorld().GetComponent<Engine::TransformComponent>(GetEntity());
        if (transform != nullptr) {
            transform->local.position = {0.0f, 1.2f, -4.0f};
            transform->local.SetEuler(0.0f, 0.0f, 0.0f);
        }
        m_yaw = 0.0f;
        m_pitch = 0.0f;
        SyncCamera();
    }

    void Update(float deltaSeconds) override {
        Engine::InputManager* input = GetWorld().GetInput();
        Engine::TransformComponent* transform =
            GetWorld().GetComponent<Engine::TransformComponent>(GetEntity());
        if (input == nullptr || transform == nullptr) {
            return;
        }

        if (input->IsMouseDown(Engine::MouseButton::Right)) {
            m_yaw += input->MouseDeltaX() * m_lookSensitivity;
            m_pitch -= input->MouseDeltaY() * m_lookSensitivity;
            m_pitch = Engine::Clamp(m_pitch, Engine::ToRadians(-80.0f), Engine::ToRadians(80.0f));
            transform->local.SetEuler(m_pitch, m_yaw, 0.0f);
        }

        const Engine::Vec3 axis = input->GetMoveAxis();
        if (axis.LengthSquared() > 0.0f) {
            const Engine::Vec3 forward = transform->local.Forward();
            const Engine::Vec3 right = transform->local.Right();
            const Engine::Vec3 up = Engine::Vec3::Up();

            Engine::Vec3 velocity =
                forward * axis.z +
                right * axis.x +
                up * axis.y;
            if (velocity.LengthSquared() > 0.0f) {
                velocity = velocity.Normalized() * m_moveSpeed * deltaSeconds;
                transform->local.position += velocity;
            }
        }

        m_fireCooldown -= deltaSeconds;
        if (input->WasActionPressed(Engine::Action::Fire) && m_fireCooldown <= 0.0f) {
            m_fireCooldown = 0.18f;
            FireProjectile(*transform);
        }

        SyncCamera();
    }

    void LateUpdate(float) override {
        SyncCamera();
    }

    void OnCollisionEnter(const Engine::CollisionInfo& collision) override {
        const Engine::TagComponent* otherTag =
            GetWorld().GetComponent<Engine::TagComponent>(collision.other);
        if (otherTag == nullptr) {
            return;
        }

        if (otherTag->tag == Engine::EntityTag::Obstacle) {
            GameSession::DamagePlayer(5);
            if (Engine::Time* time = GetWorld().GetTime()) {
                time->TriggerSlowMotion(0.25f, 0.3f);
            }
        }
    }

private:
    void FireProjectile(const Engine::TransformComponent& shipTransform) {
        ID3D11Device* device = GetWorld().GetDevice();
        if (device == nullptr) {
            return;
        }

        if (Engine::ParticleEmitterComponent* emitter =
                GetWorld().GetComponent<Engine::ParticleEmitterComponent>(GetEntity())) {
            const Engine::Vec3 muzzle =
                shipTransform.local.position + shipTransform.local.Forward() * 0.7f;
            Engine::ParticleSystem::EmitBurst(
                *emitter,
                muzzle,
                shipTransform.local.Forward(),
                10);
        }

        const Engine::Entity projectile = GetWorld().CreateEntity();

        Engine::TransformComponent transform;
        transform.local.position =
            shipTransform.local.position + shipTransform.local.Forward() * 1.1f;
        transform.local.scale = {0.18f, 0.18f, 0.45f};
        transform.local.rotation = shipTransform.local.rotation;
        GetWorld().AddComponent<Engine::TransformComponent>(projectile, transform);

        Engine::MeshComponent mesh;
        mesh.mesh = Engine::Mesh::CreateCube(device, 1.0f);
        mesh.tint = {0.55f, 1.0f, 0.35f};
        GetWorld().AddComponent<Engine::MeshComponent>(projectile, std::move(mesh));

        Engine::ColliderComponent collider;
        collider.shape = Engine::ColliderShape::Sphere;
        collider.radius = 0.22f;
        collider.isStatic = false;
        collider.isTrigger = true;
        GetWorld().AddComponent<Engine::ColliderComponent>(projectile, collider);

        Engine::TagComponent tag;
        tag.tag = Engine::EntityTag::Projectile;
        GetWorld().AddComponent<Engine::TagComponent>(projectile, tag);

        Engine::ParticleEmitterComponent sparks;
        sparks.maxParticles = 64;
        sparks.emissionRate = 0.0f;
        sparks.startColor = {0.5f, 1.0f, 1.0f};
        sparks.endColor = {0.1f, 0.2f, 1.0f};
        sparks.minSpeed = 1.0f;
        sparks.maxSpeed = 4.0f;
        sparks.directionJitter = 0.8f;
        GetWorld().AddComponent<Engine::ParticleEmitterComponent>(projectile, std::move(sparks));

        auto script = std::make_unique<ProjectileScript>();
        script->Configure(shipTransform.local.Forward(), 32.0f, 2.2f);
        GetWorld().AddScript(projectile, std::move(script));
        GetWorld().StartScripts();
    }

    void SyncCamera() {
        Engine::Camera* camera = GetWorld().GetCamera();
        Engine::TransformComponent* transform =
            GetWorld().GetComponent<Engine::TransformComponent>(GetEntity());
        if (camera == nullptr || transform == nullptr) {
            return;
        }
        camera->transform = transform->local;
    }

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_moveSpeed = 7.0f;
    float m_lookSensitivity = 0.005f;
    float m_fireCooldown = 0.0f;
};
