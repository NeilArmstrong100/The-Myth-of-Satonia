#pragma once

#include "registry.h"
#include "../main/util.h"

#include <vector>
#include <unordered_map>

struct Component
{
	virtual ~Component() = default;
	Component() = default;
};

// Generic properties

struct Transform : Component
{
	Point position = { 0.0f, 0.0f };
	Point velocity = { 0.0f, 0.0f };
	Point size = { 0.2f, 0.2f };
};

struct Sprite : Component
{
	std::byte texture_id = std::byte(5);
};

struct Collision : Component
{
	bool enabled = false;
	bool touching = false;
};

struct Speed : Component
{
	float speed = 0.0f;
};

struct Range : Component
{
	float range = 0.0f;
};

struct Useable : Component
{
	bool toggle = false;
};

struct Lock : Component
{
	bool open = false;
};

struct Inventory : Component
{
	std::vector<std::uint32_t> contents = {};
};

struct Teleport : Component
{
    int map_to_load = 0;
    Point next_position = { 0.0f, 0.0f };
    bool active = false;

    // Add a constructor to match the usage in test_1.h
    Teleport() = default;
    Teleport(int map, Point pos, bool act)
        : map_to_load(map), next_position(pos), active(act) {}
};

struct Health
{
	float health = 100.0f;
	float defense = 0.0f;
};

// Generic identifiers

struct Player : Component {};
struct Enemy : Component {};
struct Item : Component {};

// Specific identifiers

struct Chest : Component {};
struct Door : Component {};
struct Key : Component {};
struct Boots : Component {};

namespace component
{
    template<std::derived_from<Component> T>
    void iterator(std::unordered_map<Entity, T>& backup, std::vector<Entity>& entities, std::shared_ptr<Registry> registry)
    {
        for (auto& [e, comp] : backup)
        {
            if (entities.end() == std::find(entities.begin(), entities.end(), e)) 
                entities.push_back(e);
            registry->add_component<T>(e) = comp;
        }
    }
}