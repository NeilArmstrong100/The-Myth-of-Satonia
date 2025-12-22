#pragma once

#include <vector>
#include "../entity/registry.h"
#include "../entity/component.h"

namespace outskirts
{
    constexpr int WIDTH = 38, HEIGHT = 21;

    inline const int(&get_map())[HEIGHT][WIDTH]
    {
        static const int map[HEIGHT][WIDTH] =
        {
            {3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  4,  4,  4,  15, 15, 15, 15, 15, 15},
            {3,  3,  3,  3,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15, 15, 15, 15, 15},
            {3,  3,  4,  3,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15, 15, 15, 15, 15},
            {3,  4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15, 15, 15, 15},
            {3,  4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15, 15, 15, 15},
            {3,  4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15, 15, 15},
            {4,  4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15, 15, 15},
            {4,  4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  4,  15, 15, 15},
            {4,  4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15, 15},
            {3,  3,  3,  3,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15, 15},
            {3,  1,  1,  1,  18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15, 15},
            {1,  1,  1,  1,  17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 21, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15},
            {3,  1,  1,  1,  19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 17, 17, 17, 19, 19, 19, 19, 19, 19, 19, 15, 15, 15, 15, 15, 15, 4,  4,  4,  15, 15},
            {3,  3,  3,  3,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 20, 17, 21, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  4,  4 },
            {15, 4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 20, 17, 21, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  4 },
            {15, 4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 20, 17, 21, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4,  4 },
            {15, 4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 20, 17, 21, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4,  4 },
            {15, 4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 20, 17, 21, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4,  4 },
            {15, 4,  4,  4,  15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 19, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 4 },
            {15, 4,  4,  4,  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16},
            {15, 4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4 }
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
            component::iterator<Boots>(s_backup<Boots>, s_entities, registry);
            component::iterator<Useable>(s_backup<Useable>, s_entities, registry);
            component::iterator<Inventory>(s_backup<Inventory>, s_entities, registry);
            component::iterator<Chest>(s_backup<Chest>, s_entities, registry);
            component::iterator<Lock>(s_backup<Lock>, s_entities, registry);
            component::iterator<Door>(s_backup<Door>, s_entities, registry);
            component::iterator<Teleport>(s_backup<Teleport>, s_entities, registry);
            return s_entities.size();
        }

		// Each tree (5) is made of 3 log parts and 25 leaf parts; 5 * (3 + 25) + 2 = 142
        s_entities.reserve(142);

        // Door 1
        Entity door_1 = registry->create_entity();
        s_entities.push_back(door_1);
        registry->add_component<Transform>(door_1).position = { 0.0f, 1.8f };
        registry->add_component<Sprite>(door_1).texture_id = std::byte(9);
        registry->add_component<Collision>(door_1).enabled = true;
        registry->add_component<Useable>(door_1);
        registry->add_component<Lock>(door_1).open = true;
        registry->add_component<Door>(door_1);
        registry->add_component<Teleport>(door_1) = { 2, { 5.2f, 0.2f }, true };

        // Sewer
		Entity sewer = registry->create_entity();
		s_entities.push_back(sewer);
        registry->add_component<Transform>(sewer).position = { 0.4f, 3.6f };
        registry->add_component<Sprite>(sewer).texture_id = std::byte(23);
        registry->add_component<Collision>(sewer);

        // Tree 1
        Point pos_1 = { 1.6f, 2.6f };

        int leaf_map_1[5][5] =
        {
            { 33, 28, 28, 28, 34 },
            { 29, 26, 26, 26, 30 },
            { 29, 26, 26, 26, 30 },
            { 31, 26, 26, 26, 32 },
            { 35, 31, 27, 32, 35 }
        };

        // Stem
        for (int i = 0; i < 3; i++)
        {
			Entity part = registry->create_entity();
            s_entities.push_back(part);
            registry->add_component<Transform>(part).position = { pos_1.x, pos_1.y + (Tile::s_size * i) };
            
            if (i == 0)
            {
                registry->add_component<Sprite>(part).texture_id = std::byte(25);
				registry->add_component<Collision>(part).enabled = true;
            }
            else
            {
                registry->add_component<Sprite>(part).texture_id = std::byte(24);
                registry->add_component<Collision>(part).enabled = false;
            }
        }

        // Leaves
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
            {
                Entity part = registry->create_entity();
                s_entities.push_back(part);
                registry->add_component<Transform>(part).position = { (pos_1.x - 0.4f) + (Tile::s_size * j), (pos_1.y + 0.4f) + (Tile::s_size * i)};
                registry->add_component<Sprite>(part).texture_id = std::byte(leaf_map_1[i][j]);
                registry->add_component<Collision>(part).enabled = false;
            }

        // Tree 2
        Point pos_2 = { 3.0f, 2.4f };

        // Stem
        for (int i = 0; i < 3; i++)
        {
            Entity part = registry->create_entity();
            s_entities.push_back(part);
            registry->add_component<Transform>(part).position = { pos_2.x, pos_2.y + (Tile::s_size * i) };

            if (i == 0)
            {
                registry->add_component<Sprite>(part).texture_id = std::byte(25);
                registry->add_component<Collision>(part).enabled = true;
            }
            else
            {
                registry->add_component<Sprite>(part).texture_id = std::byte(24);
                registry->add_component<Collision>(part).enabled = false;
            }
        }

        // Leaves
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
            {
                Entity part = registry->create_entity();
                s_entities.push_back(part);
                registry->add_component<Transform>(part).position = { (pos_2.x - 0.4f) + (Tile::s_size * j), (pos_2.y + 0.4f) + (Tile::s_size * i) };
                registry->add_component<Sprite>(part).texture_id = std::byte(leaf_map_1[i][j]);
                registry->add_component<Collision>(part).enabled = false;
            }

        // Tree 3
        Point pos_3 = { 2.8f, 1.0f };

        // Stem
        for (int i = 0; i < 3; i++)
        {
            Entity part = registry->create_entity();
            s_entities.push_back(part);
            registry->add_component<Transform>(part).position = { pos_3.x, pos_3.y + (Tile::s_size * i) };

            if (i == 0)
            {
                registry->add_component<Sprite>(part).texture_id = std::byte(25);
                registry->add_component<Collision>(part).enabled = true;
            }
            else
            {
                registry->add_component<Sprite>(part).texture_id = std::byte(24);
                registry->add_component<Collision>(part).enabled = false;
            }
        }

        // Leaves
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
            {
                Entity part = registry->create_entity();
                s_entities.push_back(part);
                registry->add_component<Transform>(part).position = { (pos_3.x - 0.4f) + (Tile::s_size * j), (pos_3.y + 0.4f) + (Tile::s_size * i) };
                registry->add_component<Sprite>(part).texture_id = std::byte(leaf_map_1[i][j]);
                registry->add_component<Collision>(part).enabled = false;
            }

        // Tree 4
        Point pos_4 = { 5.0f, 0.6f };

        // Stem
        for (int i = 0; i < 3; i++)
        {
            Entity part = registry->create_entity();
            s_entities.push_back(part);
            registry->add_component<Transform>(part).position = { pos_4.x, pos_4.y + (Tile::s_size * i) };

            if (i == 0)
            {
                registry->add_component<Sprite>(part).texture_id = std::byte(25);
                registry->add_component<Collision>(part).enabled = true;
            }
            else
            {
                registry->add_component<Sprite>(part).texture_id = std::byte(24);
                registry->add_component<Collision>(part).enabled = false;
            }
        }

        // Leaves
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
            {
                Entity part = registry->create_entity();
                s_entities.push_back(part);
                registry->add_component<Transform>(part).position = { (pos_4.x - 0.4f) + (Tile::s_size * j), (pos_4.y + 0.4f) + (Tile::s_size * i) };
                registry->add_component<Sprite>(part).texture_id = std::byte(leaf_map_1[i][j]);
                registry->add_component<Collision>(part).enabled = false;
            }

        // Tree 5
        Point pos_5 = { 5.4f, 3.0f };

        // Stem
        for (int i = 0; i < 3; i++)
        {
            Entity part = registry->create_entity();
            s_entities.push_back(part);
            registry->add_component<Transform>(part).position = { pos_5.x, pos_5.y + (Tile::s_size * i) };

            if (i == 0)
            {
                registry->add_component<Sprite>(part).texture_id = std::byte(25);
                registry->add_component<Collision>(part).enabled = true;
            }
            else
            {
                registry->add_component<Sprite>(part).texture_id = std::byte(24);
                registry->add_component<Collision>(part).enabled = false;
            }
        }

        // Leaves
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
            {
                Entity part = registry->create_entity();
                s_entities.push_back(part);
                registry->add_component<Transform>(part).position = { (pos_5.x - 0.4f) + (Tile::s_size * j), (pos_5.y + 0.4f) + (Tile::s_size * i) };
                registry->add_component<Sprite>(part).texture_id = std::byte(leaf_map_1[i][j]);
                registry->add_component<Collision>(part).enabled = false;
            }

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