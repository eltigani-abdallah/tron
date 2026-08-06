#pragma once
#include "Components.hpp"

class Velocity : public Component
{
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    const char* GetName() const override { return "Velocity"; }
};
