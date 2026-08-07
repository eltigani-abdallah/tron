#pragma once

#include "Engine/Math/Vec3.hpp"

#include <vector>

namespace Engine {

struct Particle {
    Vec3 position = Vec3::Zero();
    Vec3 velocity = Vec3::Zero();
    Vec3 color = Vec3::One();
    float life = 0.0f;
    float maxLife = 1.0f;
    float size = 0.1f;
};

struct ParticleEmitterComponent {
    std::vector<Particle> particles;
    int maxParticles = 512;

    // Continuous emission (particles / second). 0 = burst only.
    float emissionRate = 0.0f;
    float emitAccumulator = 0.0f;

    Vec3 localOffset = Vec3::Zero();
    Vec3 direction = {0.0f, 0.0f, 1.0f};
    float directionJitter = 0.35f;

    float minSpeed = 2.0f;
    float maxSpeed = 6.0f;
    float minLife = 0.2f;
    float maxLife = 0.7f;
    float minSize = 0.04f;
    float maxSize = 0.12f;

    Vec3 startColor = {1.0f, 0.85f, 0.25f};
    Vec3 endColor = {1.0f, 0.2f, 0.05f};

    bool active = true;
};

} // namespace Engine
