#pragma once
#include <cstdint>
#include "Velocity.hpp"
#include "Transform.hpp"
#include "MeshRenderer.hpp"

struct Transform
{
    float x, y, z;
};

struct Sprite
{
    int textureId;
};

struct Collider
{
    float width, height;
};

struct Script
{
    int scriptID;
    bool isActive;
};

struct Physics
{
    float velocityX;
    float velocityY;

    float accelerationX;
    float accelerationY;

    float forceX;
    float forceY;

    float mass;

    float friction;

    float restitution;
};

#pragma once
#include <cstdint>
#include "../ECS/EntityId.hpp"

class Component
{
public:
    Component() : owner({0, 0}) {}
    virtual ~Component() = default;

    // Entité à laquelle ce composant appartient
    EntityId owner;

    // Nom du type (pour debug / outils)
    virtual const char* GetName() const = 0;
};
