#pragma once

#include "Engine/Export.hpp"
#include "Engine/States/State.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace Engine {

class ENGINE_API StateManager {
public:
    StateManager() = default;
    ~StateManager();

    StateManager(const StateManager&) = delete;
    StateManager& operator=(const StateManager&) = delete;

    void Register(const char* name, std::unique_ptr<State> state);
    void ChangeState(const char* name);
    void Clear();

    void Update(float deltaSeconds);

    State* Current() const { return m_current; }
    const char* CurrentName() const;

private:
    void ApplyPendingChange();

    std::unordered_map<std::string, std::unique_ptr<State>> m_states;
    State* m_current = nullptr;
    std::string m_pendingName;
    bool m_hasPending = false;
};

} // namespace Engine
