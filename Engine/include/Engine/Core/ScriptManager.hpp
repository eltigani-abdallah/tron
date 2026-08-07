#pragma once

#include "Engine/Core/Script.hpp"
#include "Engine/ECS/Entity.hpp"
#include "Engine/Export.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace Engine {

class World;

class ENGINE_API ScriptManager {
public:
    ScriptManager() = default;
    ScriptManager(const ScriptManager&) = delete;
    ScriptManager& operator=(const ScriptManager&) = delete;
    ScriptManager(ScriptManager&&) noexcept = default;
    ScriptManager& operator=(ScriptManager&&) noexcept = default;

    void Attach(Entity entity, std::unique_ptr<Script> script, World* world);
    void Detach(Entity entity);
    void Clear();

    Script* Get(Entity entity) const;

    void StartAll();
    void UpdateAll(float deltaSeconds);
    void LateUpdateAll(float deltaSeconds);

private:
    struct Entry {
        Entity entity = kInvalidEntity;
        std::unique_ptr<Script> script;
    };

    std::vector<Entry> m_scripts;
    std::unordered_map<Entity, std::size_t> m_lookup;
};

} // namespace Engine
