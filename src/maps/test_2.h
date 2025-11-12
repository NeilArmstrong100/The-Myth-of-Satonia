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
    std::vector<Entity> s_entities;

    void load_entities(Registry& registry)
    {
        // Door 1
        Entity door_1 = registry.create_entity();
        s_entities.push_back(door_1);
        registry.add_component<Transform>(door_1).position = { 5.4f, 0.2f };
        registry.add_component<Sprite>(door_1).texture_id = 9;
        registry.add_component<Collision>(door_1).enabled = true;
        registry.add_component<Useable>(door_1);
        registry.add_component<Lock>(door_1).open = true;
        registry.add_component<Door>(door_1);

        // Door 2
        Entity door_2 = registry.create_entity();
        s_entities.push_back(door_2);
        registry.add_component<Transform>(door_2).position = { 2.4f, 2.4f };
        registry.add_component<Sprite>(door_2).texture_id = 9;
        registry.add_component<Collision>(door_2).enabled = true;
        registry.add_component<Useable>(door_2);
        registry.add_component<Lock>(door_2).open = true;
        registry.add_component<Door>(door_2);
        registry.add_component<Teleport>(door_2) = { 1, { 2.4f, 0.2f }, true };
    }

    void clear(Registry& registry)
    {
        // Erase all components for entities created by this map
        for (Entity e : s_entities)
        {
            registry.get_all<Transform>().erase(e);
            registry.get_all<Sprite>().erase(e);
            registry.get_all<Collision>().erase(e);
            registry.get_all<Speed>().erase(e);
            registry.get_all<Enemy>().erase(e);
            registry.get_all<Item>().erase(e);
            registry.get_all<Key>().erase(e);
            registry.get_all<Boots>().erase(e);
            registry.get_all<Useable>().erase(e);
            registry.get_all<Inventory>().erase(e);
            registry.get_all<Chest>().erase(e);
            registry.get_all<Lock>().erase(e);
            registry.get_all<Door>().erase(e);
            registry.get_all<Teleport>().erase(e);
        }
        s_entities.clear();
    }
}