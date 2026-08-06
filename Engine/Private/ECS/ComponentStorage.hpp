#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>
#include "../ECS/Component.hpp"

class ComponentStorage
{
public:
    ComponentStorage() = default;
    ~ComponentStorage() = default;

    // Ajouter un composant
    template<typename T>
    void Add(std::unique_ptr<T> component)
    {
        components[typeid(T)] = std::move(component);
    }

    // Vérifier si un composant existe
    template<typename T>
    bool Has() const
    {
        return components.find(typeid(T)) != components.end();
    }

    // Récupérer un composant
    template<typename T>
    T* Get()
    {
        auto it = components.find(typeid(T));
        if (it != components.end())
            return static_cast<T*>(it->second.get());
        return nullptr;
    }

    // Supprimer un composant
    template<typename T>
    void Remove()
    {
        components.erase(typeid(T));
    }

    // Supprimer tous les composants
    void Clear()
    {
        components.clear();
    }

private:
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
};
