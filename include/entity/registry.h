#pragma once

#include <cstdint>
#include <unordered_map>
#include <shared_mutex>

// An "abstract" idea in ECS, a 32-bit unsigned integer
typedef std::uint32_t Entity;

// Global registry lock
inline constinit std::shared_mutex registry_mutex{};

class Registry {
private:
    // ID of last created entity
    Entity last_entity = 0;

    // Gets all components of type T
    template<typename T>
    std::unordered_map<Entity, T>& get_storage() 
    {
        static std::unordered_map<Entity, T> storage;
        return storage;
    }
public:
    // Creates an entity, prefix increments to return the value of last_entity after the new one was created
    Entity create_entity() { return ++last_entity; }

    // Adds a component to the entity
    template<typename T, typename... Args>
    T& add_component(Entity e, Args&&... args) 
    {
        std::unique_lock lock(registry_mutex);
        auto& store = get_storage<T>(); // Get all of the components of type T
        return store[e] = T(std::forward<Args>(args)...); // Allows initialization of new component T for entity
    }

    // Gets the specific component of the entity; Otherwise return nullptr
    template<typename T>
    T* get_component(Entity e) 
    {
        std::shared_lock lock(registry_mutex);
        auto& store = get_storage<T>(); // Get all of the components of type T
        auto pair = store.find(e);      // Get the specific component of the components of type T
        return (pair != store.end()) ? &pair->second : nullptr; // Return the component, else, nullptr
    }

    // Gets all components of type T (read-only)
    template<typename T>
    std::unordered_map<Entity, T>& get_all() 
    {
        std::shared_lock lock(registry_mutex);
        return get_storage<T>(); // Return copy while holding shared lock
    }
};
