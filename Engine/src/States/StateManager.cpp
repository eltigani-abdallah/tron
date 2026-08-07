#include "Engine/States/StateManager.hpp"

namespace Engine {

StateManager::~StateManager() {
    Clear();
}

void StateManager::Register(const char* name, std::unique_ptr<State> state) {
    if (name == nullptr || state == nullptr) {
        return;
    }

    state->m_name = name;
    state->m_manager = this;
    m_states[name] = std::move(state);
}

void StateManager::ChangeState(const char* name) {
    if (name == nullptr) {
        return;
    }
    m_pendingName = name;
    m_hasPending = true;
}

void StateManager::Clear() {
    if (m_current != nullptr) {
        m_current->Exit();
        m_current = nullptr;
    }
    m_states.clear();
    m_hasPending = false;
    m_pendingName.clear();
}

void StateManager::Update(float deltaSeconds) {
    ApplyPendingChange();
    if (m_current != nullptr) {
        m_current->Update(deltaSeconds);
    }
    ApplyPendingChange();
}

const char* StateManager::CurrentName() const {
    return m_current != nullptr ? m_current->GetName() : "";
}

void StateManager::ApplyPendingChange() {
    if (!m_hasPending) {
        return;
    }

    m_hasPending = false;
    const auto it = m_states.find(m_pendingName);
    if (it == m_states.end()) {
        return;
    }

    if (m_current != nullptr) {
        m_current->Exit();
    }

    m_current = it->second.get();
    m_current->Enter();
}

} // namespace Engine
