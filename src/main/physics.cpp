#include <thread>
#include <chrono>
#include <algorithm>
#include "physics.h"

bool util::thread::physics_done = false;
bool util::thread::running = true;
float generosity = 0.03f; // How much collision boxes are radially offset for fairness
const float COLLISION_EPSILON = 0.0001f; // Prevents glitches
Registry* registry = nullptr;
std::thread util::thread::physics_thread(run);

void start(Registry& reg)
{
    registry = &reg;
	util::thread::physics_thread.detach();
}

void run()
{
    auto target_time = std::chrono::nanoseconds(1'000'000'000 / frame_rate);
    auto last_time = std::chrono::high_resolution_clock::now();

    while (util::thread::running)
    {
        auto start_time = std::chrono::high_resolution_clock::now();
        float delta_time = std::chrono::duration<float>(start_time - last_time).count();
        last_time = start_time;

        if (util::key_manager::paused)
            continue;

        physics_update(delta_time);
        event_handler(delta_time);

        auto work_time = std::chrono::duration_cast<std::chrono::nanoseconds>(start_time - last_time);

        if (work_time < target_time)
            std::this_thread::sleep_until(start_time + target_time);
    }
}

void physics_update(const float dt)
{
    auto& transforms = registry->get_all<Transform>();
    for (auto& pair : transforms)
    {
        Entity entity = pair.first;
        Transform& transform = pair.second;
        Point& size = transform.size;
        Collision* collision = registry->get_component<Collision>(entity);

        Point velocity = transform.velocity;
        Point pos = transform.position;

        collision->touching = false;

        if (registry->get_component<Player>(entity))
        {
            using namespace util::key_manager;

            float speed = (*registry).get_component<Speed>(entity)->speed;

            for (Entity e : registry->get_component<Inventory>(entity)->contents)
                if (registry->get_component<Boots>(e)) speed *= 1.5f;

            // Triangulates the speed when moving diagonal
            if ((w && a) || (w && d) || (s && a) || (s && d))
                speed /= std::sqrt(2.0f);

            if (w) velocity.y = speed * dt;
            if (s) velocity.y = -speed * dt;
            if (!w && !s) velocity.y = 0;

            if (a) velocity.x = -speed * dt;
            if (d) velocity.x = speed * dt;
            if (!a && !d) velocity.x = 0;

            transform.velocity = velocity;
        }

        pos.x += velocity.x;

        // Horizontal colliders
        for (auto& pair : transforms)
        {
            Entity other = pair.first;
            Transform& ot = pair.second;
            Collision* oc = registry->get_component<Collision>(other);

            if (!oc->enabled || other == entity) continue;

            float tx = ot.position.x;
            float ty = ot.position.y;
            float tw = ot.size.x;
            float th = ot.size.y;

            if (pos.x + generosity <= tx + tw && pos.x + size.x - generosity >= tx &&
                pos.y + generosity <= ty + th && pos.y + size.y - generosity >= ty)
            {
                collision->touching = true;
                if (velocity.x > 0) pos.x = tx - size.x + generosity - COLLISION_EPSILON;
                else if (velocity.x < 0) pos.x = tx + tw - generosity + COLLISION_EPSILON;
                velocity.x = 0;
            }
        }

        for (auto& pair : Tile::s_tiles)
        {
            Tile& tile = pair.second;

            if (!collision->enabled) continue;

            byte id = tile.get_id();

            // Ignore colliders if tile matches these ids
            if (id == 0 || id == 1 || id == 15 || id == 16) continue;

            float tx = tile.get_position().x;
            float ty = tile.get_position().y;
            float tw = Tile::s_size;
            float th = Tile::s_size;

            if (pos.x + generosity <= tx + tw && pos.x + size.x - generosity >= tx &&
                pos.y + generosity <= ty + th && pos.y + size.y - generosity >= ty)
            {
                collision->touching = true;
                if (velocity.x > 0) pos.x = tx - size.x + generosity - COLLISION_EPSILON;
                else if (velocity.x < 0) pos.x = tx + tw - generosity + COLLISION_EPSILON;
                velocity.x = 0;
            }
        }

        pos.y += velocity.y;

        // Vertical colliders
        for (auto& pair : transforms)
        {
            Entity other = pair.first;
            Transform& ot = pair.second;
            Collision* oc = registry->get_component<Collision>(other);

            if (!oc->enabled || other == entity) continue;

            float tx = ot.position.x;
            float ty = ot.position.y;
            float tw = ot.size.x;
            float th = ot.size.y;

            if (pos.x + generosity <= tx + tw && pos.x + size.x - generosity >= tx &&
                pos.y + generosity <= ty + th && pos.y + size.y - generosity >= ty)
            {
                collision->touching = true;
                if (velocity.y > 0) pos.y = ty - size.y + generosity - COLLISION_EPSILON;
                else if (velocity.y < 0) pos.y = ty + th - generosity + COLLISION_EPSILON;
                velocity.y = 0;
            }
        }

        for (auto& pair : Tile::s_tiles)
        {
            Tile& tile = pair.second;

            if (!collision->enabled) continue;

            byte id = tile.get_id();

            // Ignore colliders if tile matches these ids
            if (id == 0 || id == 1 || id == 15 || id == 16) continue;

            float tx = tile.get_position().x;
            float ty = tile.get_position().y;
            float tw = Tile::s_size;
            float th = Tile::s_size;

            if (pos.x + generosity <= tx + tw && pos.x + size.x - generosity >= tx &&
                pos.y + generosity <= ty + th && pos.y + size.y - generosity >= ty)
            {
                collision->touching = true;
                if (velocity.y > 0) pos.y = ty - size.y + generosity - COLLISION_EPSILON;
                else if (velocity.y < 0) pos.y = ty + th - generosity + COLLISION_EPSILON;
                velocity.y = 0;
            }
        }

        transform.velocity = velocity;
        transform.position = pos;
    }
}

void event_handler(float dt)
{
    for (auto& pair : registry->get_all<Useable>()) 
    {
        Entity entity = pair.first;
        Useable& useable = pair.second;
        Transform& transform = *registry->get_component<Transform>(entity);
        
        Entity player = registry->get_all<Player>().begin()->first;
        Transform& pt = *registry->get_component<Transform>(player);

        float range = registry->get_component<Range>(player)->range;

        float px = pt.position.x;
        float py = pt.position.y;
        float pw = pt.size.x;
        float ph = pt.size.y;

        float tx = transform.position.x;
        float ty = transform.position.y;
        float tw = transform.size.x;
        float th = transform.size.y;

        if (px + (pw / 2) - range <= tx + tw && px + (pw / 2) + range >= tx &&
            py + (ph / 2) - range <= ty + th && py + (ph / 2) + range >= ty &&
            util::key_manager::space)
            useable.toggle = true;
    }

    for (auto& pair : registry->get_all<Chest>())
    {
        Entity entity = pair.first;
        Useable& useable = *registry->get_component<Useable>(entity);

        Entity player = registry->get_all<Player>().begin()->first;

        if (!useable.toggle)
            registry->get_component<Sprite>(entity)->texture_id = 6;
        else
        {
            Inventory& inventory = *registry->get_component<Inventory>(entity);
            if (inventory.contents.size() == 0) continue;
            registry->get_component<Sprite>(entity)->texture_id = 7;
            registry->get_component<Inventory>(player)->contents.push_back(inventory.contents.at(0));
            inventory.contents = {};
        }
    }

    for (auto& pair : registry->get_all<Door>())
    {
        Entity entity = pair.first;
        Useable& useable = *registry->get_component<Useable>(entity);
        Lock& lock = *registry->get_component<Lock>(entity);

        if (!lock.open)
        {
            Entity player = registry->get_all<Player>().begin()->first;

            int keys = 0;
            auto& contents = registry->get_component<Inventory>(player)->contents;
            auto& key = registry->get_all<Key>();
            for (auto& pair : key)
                for (Entity content : contents)
                    if (pair.first == content) keys++;

            if (keys == 0)
                useable.toggle = false;

            if (keys > 0 && useable.toggle)
            {
                lock.open = true;
                for (int i = 0; i < registry->get_component<Inventory>(player)->contents.size(); i++)
                    if (registry->get_component<Key>(registry->get_component<Inventory>(player)->contents[i]))
                        registry->get_component<Inventory>(player)->contents.erase(contents.begin() + i);
            }

            registry->get_component<Sprite>(entity)->texture_id = 8;
            registry->get_component<Collision>(entity)->enabled = true;
        }
        else
        {
            if (!useable.toggle)
            {
                registry->get_component<Sprite>(entity)->texture_id = 9;
                registry->get_component<Collision>(entity)->enabled = true;
            }
            else
            {
                registry->get_component<Sprite>(entity)->texture_id = 10;
                registry->get_component<Collision>(entity)->enabled = false;
            }
        }
    }

    for (auto& pair : registry->get_all<Teleport>())
    {
        Entity entity = pair.first;
        Teleport& teleport = pair.second;
        Transform& transform = *registry->get_component<Transform>(entity);

        if (!teleport.active) continue;

        Entity player = registry->get_all<Player>().begin()->first;
        Transform& pt = *registry->get_component<Transform>(player);
        
        float px = pt.position.x;
        float py = pt.position.y;
        float pw = pt.size.x;
        float ph = pt.size.y;

        float tx = transform.position.x;
        float ty = transform.position.y;
        float tw = transform.size.x;
        float th = transform.size.y;

        if (px + (pw / 2) <= tx + tw && px + (pw / 2) >= tx &&
            py + (ph / 2) <= ty + th && py + (ph / 2) >= ty)
        {
            pt.position = teleport.next_position;
            util::map::map_to_load = teleport.map_to_load;
            util::map::map_loaded = false;
        }
    }
}