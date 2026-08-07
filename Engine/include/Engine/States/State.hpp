#pragma once

#include "Engine/Export.hpp"

namespace Engine {

class StateManager;

class ENGINE_API State {
public:
    virtual ~State();

    virtual void Enter() {}
    virtual void Update(float deltaSeconds) { (void)deltaSeconds; }
    virtual void Exit() {}

    const char* GetName() const { return m_name; }
    StateManager* GetManager() const { return m_manager; }

private:
    friend class StateManager;

    const char* m_name = "";
    StateManager* m_manager = nullptr;
};

} // namespace Engine
