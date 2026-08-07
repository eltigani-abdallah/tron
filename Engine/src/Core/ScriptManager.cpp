#include "Engine/Core/ScriptManager.hpp"

namespace Engine {

void ScriptManager::Attach(Entity entity, std::unique_ptr<Script> script, World* world) {
    if (entity == kInvalidEntity || script == nullptr || world == nullptr) {
        return;
    }

    Detach(entity);

    script->m_entity = entity;
    script->m_world = world;
    script->m_started = false;

    m_lookup[entity] = m_scripts.size();
    m_scripts.push_back(Entry{entity, std::move(script)});
}

void ScriptManager::Detach(Entity entity) {
    const auto it = m_lookup.find(entity);
    if (it == m_lookup.end()) {
        return;
    }

    const std::size_t index = it->second;
    const std::size_t last = m_scripts.size() - 1;
    if (index != last) {
        m_lookup[m_scripts[last].entity] = index;
        m_scripts[index] = std::move(m_scripts[last]);
    }

    m_scripts.pop_back();
    m_lookup.erase(it);
}

void ScriptManager::Clear() {
    m_scripts.clear();
    m_lookup.clear();
}

Script* ScriptManager::Get(Entity entity) const {
    const auto it = m_lookup.find(entity);
    if (it == m_lookup.end()) {
        return nullptr;
    }
    return m_scripts[it->second].script.get();
}

void ScriptManager::StartAll() {
    for (Entry& entry : m_scripts) {
        if (entry.script != nullptr && !entry.script->m_started) {
            entry.script->Start();
            entry.script->m_started = true;
        }
    }
}

void ScriptManager::UpdateAll(float deltaSeconds) {
    for (Entry& entry : m_scripts) {
        if (entry.script != nullptr && entry.script->m_started) {
            entry.script->Update(deltaSeconds);
        }
    }
}

void ScriptManager::LateUpdateAll(float deltaSeconds) {
    for (Entry& entry : m_scripts) {
        if (entry.script != nullptr && entry.script->m_started) {
            entry.script->LateUpdate(deltaSeconds);
        }
    }
}

} // namespace Engine
