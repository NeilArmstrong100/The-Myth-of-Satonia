#pragma once

#include <vector>
#include <typeindex>

namespace test_1 
{
    constexpr int WIDTH = 26, HEIGHT = 11;

    inline const int(&get_map())[HEIGHT][WIDTH]
    {
        static const int map[HEIGHT][WIDTH] = 
        {
            {0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 2, 1, 1, 1, 2, 2, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 2, 2, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3},
            {2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 3},
            {2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 4, 4, 4, 1, 3},
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 4, 4, 4, 1, 3},
            {2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 1, 3},
            {2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 4, 4, 4, 1, 3},
            {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 4, 3, 3, 3}
        };
        return map;
    }

    // Track entities created by this map to remove them in clear()
    template<std::derived_from<Component> T>
    static inline std::unordered_map<Entity, T> s_backup{};
    static inline std::vector<Entity> s_entities{};

    static inline bool s_loaded = false;

    size_t load_entities(std::shared_ptr<Registry> registry)
    {
        if (s_loaded)
        {
            component::iterator<Transform>(s_backup<Transform>, s_entities, registry);
            component::iterator<Sprite>(s_backup<Sprite>, s_entities, registry);
            component::iterator<Collision>(s_backup<Collision>, s_entities, registry);
            component::iterator<Speed>(s_backup<Speed>, s_entities, registry);
            component::iterator<Enemy>(s_backup<Enemy>, s_entities, registry);
            component::iterator<Item>(s_backup<Item>, s_entities, registry);
            component::iterator<Key>(s_backup<Key>, s_entities, registry);
            component::iterator<Useable>(s_backup<Useable>, s_entities, registry);
            component::iterator<Inventory>(s_backup<Inventory>, s_entities, registry);
            component::iterator<Chest>(s_backup<Chest>, s_entities, registry);
            component::iterator<Lock>(s_backup<Lock>, s_entities, registry);
            component::iterator<Door>(s_backup<Door>, s_entities, registry);
            component::iterator<Teleport>(s_backup<Teleport>, s_entities, registry);
            return s_entities.size();
        }

		s_entities.reserve(11);

        // Enemy
        Entity enemy = registry->create_entity();
        s_entities.push_back(enemy);
        registry->add_component<Transform>(enemy).position = { 1.4f, 1.6f };
        registry->add_component<Sprite>(enemy).texture_id = std::byte(22);
        registry->add_component<Collision>(enemy).enabled = true;
        registry->add_component<Speed>(enemy).speed = 0.3f;
        registry->add_component<Enemy>(enemy);

        // Key
        Entity key = registry->create_entity();
        s_entities.push_back(key);
        registry->add_component<Sprite>(key).texture_id = std::byte(11);
        registry->add_component<Item>(key);
        registry->add_component<Key>(key);

        // Power Boots
        Entity power = registry->create_entity();
        s_entities.push_back(power);
        registry->add_component<Item>(power);
        registry->add_component<Boots>(power);

        // Chest 1
        Entity chest_1 = registry->create_entity();
        s_entities.push_back(chest_1);
        registry->add_component<Transform>(chest_1).position = { 0.4f, 0.6f };
        registry->add_component<Sprite>(chest_1).texture_id = std::byte(6);
        registry->add_component<Collision>(chest_1).enabled = true;
        registry->add_component<Useable>(chest_1);
        registry->add_component<Inventory>(chest_1).contents = { key };
        registry->add_component<Chest>(chest_1);

        // Chest 2
        Entity chest_2 = registry->create_entity();
        s_entities.push_back(chest_2);
        registry->add_component<Transform>(chest_2).position = { 4.8f, 0.2f };
        registry->add_component<Sprite>(chest_2).texture_id = std::byte(6);
        registry->add_component<Collision>(chest_2).enabled = true;
        registry->add_component<Useable>(chest_2);
        registry->add_component<Inventory>(chest_2).contents = { key };
        registry->add_component<Chest>(chest_2);

        // Chest 3
        Entity chest_3 = registry->create_entity();
        s_entities.push_back(chest_3);
        registry->add_component<Transform>(chest_3).position = { 4.0f, 0.2f };
        registry->add_component<Sprite>(chest_3).texture_id = std::byte(6);
        registry->add_component<Collision>(chest_3).enabled = true;
        registry->add_component<Useable>(chest_3);
        registry->add_component<Inventory>(chest_3).contents = { power };
        registry->add_component<Chest>(chest_3);

        // Door 1
        Entity door_1 = registry->create_entity();
        s_entities.push_back(door_1);
        registry->add_component<Transform>(door_1).position = { 2.4f, 1.2f };
        registry->add_component<Sprite>(door_1).texture_id = std::byte(9);
        registry->add_component<Collision>(door_1).enabled = true;
        registry->add_component<Useable>(door_1);
        registry->add_component<Lock>(door_1).open = true;
        registry->add_component<Door>(door_1);

        // Door 2
        Entity door_2 = registry->create_entity();
        s_entities.push_back(door_2);
        registry->add_component<Transform>(door_2).position = { 3.8f, 0.4f };
        registry->add_component<Sprite>(door_2).texture_id = std::byte(9);
        registry->add_component<Collision>(door_2).enabled = true;
        registry->add_component<Useable>(door_2);
        registry->add_component<Lock>(door_2).open = false;
        registry->add_component<Door>(door_2);

        // Door 3
        Entity door_3 = registry->create_entity();
        s_entities.push_back(door_3);
        registry->add_component<Transform>(door_3).position = { 2.4f, 0.0f };
        registry->add_component<Sprite>(door_3).texture_id = std::byte(9);
        registry->add_component<Collision>(door_3).enabled = true;
        registry->add_component<Useable>(door_3);
        registry->add_component<Lock>(door_3).open = false;
        registry->add_component<Door>(door_3);
        registry->add_component<Teleport>(door_3) = { 2, { 2.4f, 2.2f }, true };

        // Door 4
        Entity door_4 = registry->create_entity();
        s_entities.push_back(door_4);
        registry->add_component<Transform>(door_4).position = { 1.4f, 1.2f };
        registry->add_component<Sprite>(door_4).texture_id = std::byte(9);
        registry->add_component<Collision>(door_4).enabled = true;
        registry->add_component<Useable>(door_4);
        registry->add_component<Lock>(door_4).open = true;
        registry->add_component<Door>(door_4);

        // Door 5
        Entity door_5 = registry->create_entity();
        s_entities.push_back(door_5);
        registry->add_component<Transform>(door_5).position = { 0.8f, 0.6f };
        registry->add_component<Sprite>(door_5).texture_id = std::byte(9);
        registry->add_component<Collision>(door_5).enabled = true;
        registry->add_component<Useable>(door_5);
        registry->add_component<Lock>(door_5).open = true;
        registry->add_component<Door>(door_5);

        s_loaded = true;

		return s_entities.size();
    }

    void clear(std::shared_ptr<Registry> registry)
    {
        for (Entity e : s_entities)
        {
            if (auto* tr = registry->get_component<Transform>(e)) s_backup<Transform>[e] = *tr;
            if (auto* sp = registry->get_component<Sprite>(e)) s_backup<Sprite>[e] = *sp;
            if (auto* col = registry->get_component<Collision>(e)) s_backup<Collision>[e] = *col;
            if (auto* spd = registry->get_component<Speed>(e)) s_backup<Speed>[e] = *spd;
            if (auto* en = registry->get_component<Enemy>(e)) s_backup<Enemy>[e] = *en;
            if (auto* it = registry->get_component<Item>(e)) s_backup<Item>[e] = *it;
            if (auto* ky = registry->get_component<Key>(e)) s_backup<Key>[e] = *ky;
            if (auto* us = registry->get_component<Useable>(e)) s_backup<Useable>[e] = *us;
            if (auto* inv = registry->get_component<Inventory>(e)) s_backup<Inventory>[e] = *inv;
            if (auto* ch = registry->get_component<Chest>(e)) s_backup<Chest>[e] = *ch;
            if (auto* lk = registry->get_component<Lock>(e)) s_backup<Lock>[e] = *lk;
            if (auto* dr = registry->get_component<Door>(e)) s_backup<Door>[e] = *dr;
            if (auto* tp = registry->get_component<Teleport>(e)) s_backup<Teleport>[e] = *tp;

            registry->get_all<Transform>().erase(e);
            registry->get_all<Sprite>().erase(e);
            registry->get_all<Collision>().erase(e);
            registry->get_all<Speed>().erase(e);
            registry->get_all<Enemy>().erase(e);
            registry->get_all<Item>().erase(e);
            registry->get_all<Key>().erase(e);
            registry->get_all<Useable>().erase(e);
            registry->get_all<Inventory>().erase(e);
            registry->get_all<Chest>().erase(e);
            registry->get_all<Lock>().erase(e);
            registry->get_all<Door>().erase(e);
            registry->get_all<Teleport>().erase(e);
        }
        s_entities.clear();
    }
}