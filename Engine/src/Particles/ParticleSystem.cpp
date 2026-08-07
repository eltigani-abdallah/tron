#include "Engine/Particles/ParticleSystem.hpp"

#include "Engine/ECS/Components.hpp"
#include "Engine/ECS/World.hpp"
#include "Engine/Render/Camera.hpp"
#include "Engine/Render/Renderer.hpp"
#include "Engine/Render/Vertex.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

namespace Engine {
namespace {

Vec3 LerpColor(const Vec3& a, const Vec3& b, float t) {
    return a + (b - a) * t;
}

} // namespace

float ParticleSystem::RandomRange(float minValue, float maxValue) {
    const float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    return minValue + (maxValue - minValue) * t;
}

Vec3 ParticleSystem::RandomUnitVector() {
    const float x = RandomRange(-1.0f, 1.0f);
    const float y = RandomRange(-1.0f, 1.0f);
    const float z = RandomRange(-1.0f, 1.0f);
    Vec3 v{x, y, z};
    if (v.LengthSquared() < 0.0001f) {
        return Vec3::Up();
    }
    return v.Normalized();
}

void ParticleSystem::EmitOne(
    ParticleEmitterComponent& emitter,
    const Vec3& worldPosition,
    const Vec3& baseDirection) {
    if (static_cast<int>(emitter.particles.size()) >= emitter.maxParticles) {
        return;
    }

    Vec3 dir = baseDirection;
    if (dir.LengthSquared() < 0.0001f) {
        dir = Vec3::Forward();
    }
    dir = dir.Normalized();
    dir = (dir + RandomUnitVector() * emitter.directionJitter).Normalized();

    Particle particle;
    particle.position = worldPosition;
    particle.velocity = dir * RandomRange(emitter.minSpeed, emitter.maxSpeed);
    particle.maxLife = RandomRange(emitter.minLife, emitter.maxLife);
    particle.life = particle.maxLife;
    particle.size = RandomRange(emitter.minSize, emitter.maxSize);
    particle.color = emitter.startColor;
    emitter.particles.push_back(particle);
}

void ParticleSystem::EmitBurst(
    ParticleEmitterComponent& emitter,
    const Vec3& worldPosition,
    const Vec3& baseDirection,
    int count) {
    for (int i = 0; i < count; ++i) {
        EmitOne(emitter, worldPosition, baseDirection);
    }
}

void ParticleSystem::Update(World& world, float deltaSeconds) {
    if (deltaSeconds <= 0.0f) {
        return;
    }

    for (Entity entity : world.Entities()) {
        ParticleEmitterComponent* emitter = world.GetComponent<ParticleEmitterComponent>(entity);
        if (emitter == nullptr || !emitter->active) {
            continue;
        }

        const TransformComponent* transform = world.GetComponent<TransformComponent>(entity);
        const Vec3 origin = transform != nullptr
            ? transform->local.position + transform->local.rotation.Rotate(emitter->localOffset)
            : emitter->localOffset;

        Vec3 emitDir = emitter->direction;
        if (transform != nullptr) {
            emitDir = transform->local.rotation.Rotate(emitter->direction);
        }

        if (emitter->emissionRate > 0.0f) {
            emitter->emitAccumulator += emitter->emissionRate * deltaSeconds;
            while (emitter->emitAccumulator >= 1.0f) {
                EmitOne(*emitter, origin, emitDir);
                emitter->emitAccumulator -= 1.0f;
            }
        }

        for (Particle& particle : emitter->particles) {
            particle.life -= deltaSeconds;
            particle.position += particle.velocity * deltaSeconds;
            particle.velocity.y -= 2.5f * deltaSeconds;

            const float t = 1.0f - std::max(particle.life, 0.0f) / std::max(particle.maxLife, 0.0001f);
            particle.color = LerpColor(emitter->startColor, emitter->endColor, t);
        }

        emitter->particles.erase(
            std::remove_if(
                emitter->particles.begin(),
                emitter->particles.end(),
                [](const Particle& p) { return p.life <= 0.0f; }),
            emitter->particles.end());
    }
}

void ParticleSystem::Render(Renderer& renderer, const Camera& camera, World& world) const {
    const Vec3 camRight = camera.transform.Right();
    const Vec3 camUp = camera.transform.Up();
    const Mat4 viewProj = camera.ViewProjectionMatrix();

    std::vector<Vertex> vertices;
    vertices.reserve(512);

    for (Entity entity : world.Entities()) {
        const ParticleEmitterComponent* emitter = world.GetComponent<ParticleEmitterComponent>(entity);
        if (emitter == nullptr) {
            continue;
        }

        for (const Particle& particle : emitter->particles) {
            const float lifeRatio = std::max(particle.life, 0.0f) / std::max(particle.maxLife, 0.0001f);
            const float alpha = lifeRatio;
            const float half = particle.size * 0.5f;
            const Vec3 right = camRight * half;
            const Vec3 up = camUp * half;
            const Vec3 p = particle.position;

            const Vec3 c0 = p - right - up;
            const Vec3 c1 = p + right - up;
            const Vec3 c2 = p + right + up;
            const Vec3 c3 = p - right + up;

            const Vertex v0{c0.x, c0.y, c0.z, particle.color.x, particle.color.y, particle.color.z, alpha};
            const Vertex v1{c1.x, c1.y, c1.z, particle.color.x, particle.color.y, particle.color.z, alpha};
            const Vertex v2{c2.x, c2.y, c2.z, particle.color.x, particle.color.y, particle.color.z, alpha};
            const Vertex v3{c3.x, c3.y, c3.z, particle.color.x, particle.color.y, particle.color.z, alpha};

            vertices.push_back(v0);
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v0);
            vertices.push_back(v2);
            vertices.push_back(v3);
        }
    }

    if (!vertices.empty()) {
        renderer.DrawWorldTriangles(vertices.data(), static_cast<unsigned int>(vertices.size()), viewProj);
    }
}

} // namespace Engine
