#pragma once

#include "Engine/Math/Transform.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Render/Mesh.hpp"

namespace Engine {

enum class EntityTag {
    None,
    Player,
    Enemy,
    Projectile,
    Obstacle
};

struct TransformComponent {
    Transform local;
};

struct MeshComponent {
    Mesh mesh;
    Vec3 tint = Vec3::One();
};

struct TagComponent {
    EntityTag tag = EntityTag::None;
};

} // namespace Engine
