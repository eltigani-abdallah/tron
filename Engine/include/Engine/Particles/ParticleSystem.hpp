#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Particles/ParticleEmitter.hpp"

namespace Engine {

class Camera;
class Renderer;
class World;

class ENGINE_API ParticleSystem {
public:
    void Update(World& world, float deltaSeconds);
    void Render(Renderer& renderer, const Camera& camera, World& world) const;

    static void EmitBurst(
        ParticleEmitterComponent& emitter,
        const Vec3& worldPosition,
        const Vec3& baseDirection,
        int count);

private:
    static void EmitOne(
        ParticleEmitterComponent& emitter,
        const Vec3& worldPosition,
        const Vec3& baseDirection);

    static float RandomRange(float minValue, float maxValue);
    static Vec3 RandomUnitVector();
};

} // namespace Engine
