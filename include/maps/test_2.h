#pragma once

namespace test_2 
{
    constexpr int WIDTH = 28, HEIGHT = 13;

    inline const int(&get_map())[HEIGHT][WIDTH]
    {
        static const int map[HEIGHT][WIDTH] = 
        {
            {3, 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  1,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  3,  3,  3 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 4,  4,  4,  16, 3 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 4,  4,  4,  16, 3 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 4,  4,  4,  3 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 3,  3,  3,  15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 4,  4,  4,  3 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 3,  4,  4,  4,  3,  15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 4,  4,  3 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 2,  2,  2,  2,  2,  15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 4,  4,  4 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 3,  4,  4,  4,  3,  15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 4,  4 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 3,  3,  3,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 4,  4 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 3 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 3 },
            {3, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
            {3, 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3 }
        };
        return map;
    }

    // Track entities created by this map to remove them in clear()
    template<std::derived_from<Component> T>
    static inline std::unordered_map<Entity, T> s_backup{};
    static inline std::vector<Entity> s_entities{};

    static inline bool s_loaded = false;

    void load_entities(std::shared_ptr<Registry> registry)
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
            component::iterator<Boots>(s_backup<Boots>, s_entities, registry);
            component::iterator<Useable>(s_backup<Useable>, s_entities, registry);
            component::iterator<Inventory>(s_backup<Inventory>, s_entities, registry);
            component::iterator<Chest>(s_backup<Chest>, s_entities, registry);
            component::iterator<Lock>(s_backup<Lock>, s_entities, registry);
            component::iterator<Door>(s_backup<Door>, s_entities, registry);
            component::iterator<Teleport>(s_backup<Teleport>, s_entities, registry);
            return;
        }

        // Door 1
        Entity door_1 = registry->create_entity();
        s_entities.push_back(door_1);
        registry->add_component<Transform>(door_1).position = { 5.4f, 0.2f };
        registry->add_component<Sprite>(door_1).texture_id = std::byte(9);
        registry->add_component<Collision>(door_1).enabled = true;
        registry->add_component<Useable>(door_1);
        registry->add_component<Lock>(door_1).open = true;
        registry->add_component<Door>(door_1);
        registry->add_component<Teleport>(door_1) = { 3, { 0.2f, 1.8f }, true };

        // Door 2
        Entity door_2 = registry->create_entity();
        s_entities.push_back(door_2);
        registry->add_component<Transform>(door_2).position = { 2.4f, 2.4f };
        registry->add_component<Sprite>(door_2).texture_id = std::byte(9);
        registry->add_component<Collision>(door_2).enabled = true;
        registry->add_component<Useable>(door_2);
        registry->add_component<Lock>(door_2).open = true;
        registry->add_component<Door>(door_2);
        registry->add_component<Teleport>(door_2) = { 1, { 2.4f, 0.2f }, true };

        s_loaded = true;
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
            if (auto* bt = registry->get_component<Boots>(e)) s_backup<Boots>[e] = *bt;
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
            registry->get_all<Boots>().erase(e);
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