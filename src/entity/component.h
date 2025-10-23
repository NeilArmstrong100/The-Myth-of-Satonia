#pragma once

#include "../main/util.h"
#include <vector>

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
	byte texture_id = 5;
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

// Generic identifiers

struct Player : Component {};
struct Enemy : Component {};
struct Item : Component {};

// Specific identifiers

struct Chest : Component {};
struct Door : Component {};
struct Key : Component {};
struct Boots : Component {};