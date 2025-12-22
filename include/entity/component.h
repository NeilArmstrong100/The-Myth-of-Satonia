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

struct Transform : public Component
{
	Point position = { 0.0f, 0.0f };
	Point velocity = { 0.0f, 0.0f };
	Point size = { 0.2f, 0.2f };
};

struct Sprite : public Component
{
	std::byte texture_id = std::byte(5);
};

struct Collision : public Component
{
	bool enabled = false;
	bool touching = false;
};

struct Speed : public Component
{
	float speed = 0.0f;
};

struct Range : public Component
{
	float range = 0.0f;
};

struct Useable : public Component
{
	bool toggle = false;
};

struct Lock : public Component
{
	bool open = false;
};

struct Inventory : public Component
{
	std::vector<std::uint32_t> contents = {};
    bool opened = false;
};

struct Teleport : public Component
{
    int map_to_load = 0;
    Point next_position = { 0.0f, 0.0f };
    bool active = false;

    // Add a constructor to match the usage in test_1.h
    Teleport() = default;
    Teleport(int map, Point pos, bool act)
        : map_to_load(map), next_position(pos), active(act) {}
};

struct Health : public Component
{
	float health = 100.0f;
	float defense = 0.0f;
};

// Generic identifiers

struct Player : public Component {};
struct Enemy : public Component {};
struct Item : public Component {};

// Specific identifiers

struct Chest : public Component {};
struct Door : public Component {};
struct Key : public Component {};
struct Boots : public Component {};

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