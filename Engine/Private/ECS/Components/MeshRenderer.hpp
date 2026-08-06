#pragma once
#include "Components.hpp"

class MeshRenderer : public Component
{
public:
    int meshId = -1;        
    int materialId = -1;    

    bool visible = true;

    const char* GetName() const override { return "MeshRender"; }
};
