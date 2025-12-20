#include <thread>
#include <algorithm>
#include "main/physics.h"

const std::vector<std::byte> ID_BLACKLIST = {
    std::byte(0),
    std::byte(1),
    std::byte(15),
    std::byte(16),
    std::byte(17),
    std::byte(18),
    std::byte(19),
    std::byte(20),
    std::byte(21)
};

constexpr float GENEROSITY = 0.03f; // How much collision boxes are radially offset for fairness
constexpr float COLLISION_EPSILON = 0.0001f; // Prevents glitches
float util::time::day = 0.0f;
std::thread util::thread::physics_thread{};
extern std::shared_ptr<Registry> registry;
bool util::time::sun_rise = true,
     util::thread::physics_done = false,
     util::thread::running = true;

void start(std::shared_ptr<Registry> reg)
{
    registry = reg;
    util::thread::physics_thread = std::thread(run);
    util::thread::physics_thread.detach();
}

void run()
{
    auto target_time = std::chrono::nanoseconds(1'000'000'000 / frame_rate);
    auto last_time = std::chrono::high_resolution_clock::now();

    while (util::thread::running)
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta_time = std::chrono::duration<float>(current_time - last_time).count();

        // While paused or waiting, don't advance physics but avoid spinning the CPU hard
        if (util::key_manager::paused || registry == nullptr || !util::map::map_loaded)
        {
            last_time = current_time;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        try
        {
            physics_update(delta_time);
            event_handler(delta_time);
        }
        catch (const std::exception& e)
        { std::println(stderr, "Physics thread error: {}", e.what()); }

        if (delta_time < std::chrono::duration<float>(target_time).count())
            std::this_thread::sleep_until(current_time + target_time);

        // Update last_time to the start of this frame for accurate delta on next loop
        last_time = current_time;
    }
}

void physics_update(const float dt)
{
    // Prevents concurrent mutation from load/clear.
    std::shared_lock lock(registry_mutex);

	// Create a copy of tiles to avoid reading and writing to Tile::s_tiles concurrently
    auto tiles = Tile::s_tiles;
	auto& transforms = registry->get_all<Transform>();

    for (auto& pair : transforms)
    {
        Entity entity = pair.first;
        Transform& transform = pair.second;
        Collision* collision = registry->get_component<Collision>(entity);
        if (!collision) continue;
        Point size = transform.size;

        Point velocity = transform.velocity;
        Point pos = transform.position;

        collision->touching = false;

        if (registry->get_component<Player>(entity))
        {
            using namespace util::key_manager;

            float speed = registry->get_component<Speed>(entity)->speed;

            auto* inv = registry->get_component<Inventory>(entity);
            if (inv)
                for (Entity e : inv->contents)
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

        horizontal_colliders(tiles, transforms, transform, collision, pos, velocity, size, entity);
		vertical_colliders(tiles, transforms, transform, collision, pos, velocity, size, entity);

        transform.velocity = velocity;
        transform.position = pos;
    }
}

void event_handler(const float dt)
{
    // Hold shared lock while reading components
    std::shared_lock lock(registry_mutex);

    using namespace util::time;

    // Simulate day/night cycle
    if (sun_rise && day < 0.8f)
        day += 0.01f * dt;
    else if (sun_rise && day >= 0.8f)
        sun_rise = false;
    else if (!sun_rise && day > 0.0f)
		day -= 0.01f * dt;
	else if (!sun_rise && day <= 0.0f)
        sun_rise = true;

    useable_event();
    chest_event();
    door_event();
    teleport_event();
}

void horizontal_colliders(auto& tiles, auto& transforms, Transform& transform, Collision* collision, Point& pos, Point& velocity, Point& size, Entity entity)
{
    pos.x += velocity.x;

    // Horizontal colliders (other entities)
    for (auto& pair : transforms)
    {
        Entity other = pair.first;
        Transform& ot = pair.second;
        Collision* oc = registry->get_component<Collision>(other);

        if (!oc) continue;
        if (!oc->enabled || other == entity) continue;

        float tx = ot.position.x;
        float ty = ot.position.y;
        float tw = ot.size.x;
        float th = ot.size.y;

        if (pos.x + GENEROSITY <= tx + tw && pos.x + size.x - GENEROSITY >= tx &&
            pos.y + GENEROSITY <= ty + th && pos.y + size.y - GENEROSITY >= ty)
        {
            collision->touching = true;
            if (velocity.x > 0) pos.x = tx - size.x + GENEROSITY - COLLISION_EPSILON;
            else if (velocity.x < 0) pos.x = tx + tw - GENEROSITY + COLLISION_EPSILON;
            velocity.x = 0;
        }
    }

    // Horizontal colliders (tiles)
    for (auto& pair : tiles)
    {
        Tile& tile = pair.second;

        if (!collision->enabled) continue;

        std::byte id = tile.get_id();

        // Ignore colliders if tile matches these ids
        if (std::find(ID_BLACKLIST.begin(), ID_BLACKLIST.end(), id) != ID_BLACKLIST.end()) continue;

        float tx = tile.get_position().x;
        float ty = tile.get_position().y;
        float tw = Tile::s_size;
        float th = Tile::s_size;

        if (pos.x + GENEROSITY <= tx + tw && pos.x + size.x - GENEROSITY >= tx &&
            pos.y + GENEROSITY <= ty + th && pos.y + size.y - GENEROSITY >= ty)
        {
            collision->touching = true;
            if (velocity.x > 0) pos.x = tx - size.x + GENEROSITY - COLLISION_EPSILON;
            else if (velocity.x < 0) pos.x = tx + tw - GENEROSITY + COLLISION_EPSILON;
            velocity.x = 0;
        }
    }
}

void vertical_colliders(auto& tiles, auto& transforms, Transform& transform, Collision* collision, Point& pos, Point& velocity, Point& size, Entity entity)
{
    pos.y += velocity.y;

    // Vertical colliders (other entities)
    for (auto& pair : transforms)
    {
        Entity other = pair.first;
        Transform& ot = pair.second;
        Collision* oc = registry->get_component<Collision>(other);
        if (!oc) continue; // safety
        if (!oc->enabled || other == entity) continue;

        float tx = ot.position.x;
        float ty = ot.position.y;
        float tw = ot.size.x;
        float th = ot.size.y;

        if (pos.x + GENEROSITY <= tx + tw && pos.x + size.x - GENEROSITY >= tx &&
            pos.y + GENEROSITY <= ty + th && pos.y + size.y - GENEROSITY >= ty)
        {
            collision->touching = true;
            if (velocity.y > 0) pos.y = ty - size.y + GENEROSITY - COLLISION_EPSILON;
            else if (velocity.y < 0) pos.y = ty + th - GENEROSITY + COLLISION_EPSILON;
            velocity.y = 0;
        }
    }

    // Vertical colliders (tiles)
    for (auto& pair : tiles)
    {
        Tile& tile = pair.second;

        if (!collision->enabled) continue;

        std::byte id = tile.get_id();

        // Ignore colliders if tile matches these ids
        if (std::find(ID_BLACKLIST.begin(), ID_BLACKLIST.end(), id) != ID_BLACKLIST.end()) continue;

        float tx = tile.get_position().x;
        float ty = tile.get_position().y;
        float tw = Tile::s_size;
        float th = Tile::s_size;

        if (pos.x + GENEROSITY <= tx + tw && pos.x + size.x - GENEROSITY >= tx &&
            pos.y + GENEROSITY <= ty + th && pos.y + size.y - GENEROSITY >= ty)
        {
            collision->touching = true;
            if (velocity.y > 0) pos.y = ty - size.y + GENEROSITY - COLLISION_EPSILON;
            else if (velocity.y < 0) pos.y = ty + th - GENEROSITY + COLLISION_EPSILON;
            velocity.y = 0;
        }
    }
}

// Events
void useable_event()
{
    // Useable objects
    auto& useables = registry->get_all<Useable>();
    for (auto& pair : useables)
    {
        Entity entity = pair.first;
        Sprite* sp = registry->get_component<Sprite>(entity);
        if (!sp || sp->texture_id == std::byte(35)) continue;

        Entity player = registry->get_all<Player>().empty() ? 0 : registry->get_all<Player>().begin()->first;
        if (player == 0) continue;
        Transform* transform = registry->get_component<Transform>(entity);
        Transform* pt = registry->get_component<Transform>(player);
        Useable& useable = pair.second;
        Range* pr = registry->get_component<Range>(player);
        if (!transform || !pt || !pr) continue;

        float range = pr->range;

        float px = pt->position.x;
        float py = pt->position.y;
        float pw = pt->size.x;
        float ph = pt->size.y;

        float tx = transform->position.x;
        float ty = transform->position.y;
        float tw = transform->size.x;
        float th = transform->size.y;

        // If within range of object and space is pressed, toggle useable
        if (px + (pw / 2) - range <= tx + tw && px + (pw / 2) + range >= tx &&
            py + (ph / 2) - range <= ty + th && py + (ph / 2) + range >= ty &&
            util::key_manager::space)
            useable.toggle = true;
    }
}

void chest_event()
{
    // Chests
    for (auto& pair : registry->get_all<Chest>())
    {
        Entity entity = pair.first;
        Sprite* sp = registry->get_component<Sprite>(entity);
        if (!sp || sp->texture_id == std::byte(35)) continue;

        Useable* useable = registry->get_component<Useable>(entity);
        if (!useable) continue;

        Entity player = registry->get_all<Player>().empty() ? 0 : registry->get_all<Player>().begin()->first;
        if (player == 0) continue;
        Inventory* pi = registry->get_component<Inventory>(player); // Player inventory, not PI
        if (!pi) continue;

        Inventory* inv = registry->get_component<Inventory>(entity);

        // If chest is opened, transfer first item in chest to player inventory
        if (useable->toggle && !inv->opened)
        {
            if (!inv || inv->contents.empty()) continue;
            Sprite* cs = registry->get_component<Sprite>(entity);
            if (cs) cs->texture_id = std::byte(7);
            pi->contents.push_back(inv->contents.at(0));
            inv->contents.clear();
            inv->opened = true;
        }
    }
}

void door_event()
{
    // Doors
    for (auto& pair : registry->get_all<Door>())
    {
        Entity entity = pair.first;

        Sprite* sp = registry->get_component<Sprite>(entity);
        if (!sp || sp->texture_id == std::byte(35)) continue;

        Useable* useable = registry->get_component<Useable>(entity);
        Lock* lock = registry->get_component<Lock>(entity);
        if (!useable || !lock) continue;

        // If door is locked
        if (!lock->open)
        {
            Entity player = registry->get_all<Player>().empty() ? 0 : registry->get_all<Player>().begin()->first;
            if (player == 0) continue;

            auto* contents = registry->get_component<Inventory>(player);
            if (!contents) continue;

            // Count keys by inspecting player's inventory directly
            int keys = 0;
            for (Entity content : contents->contents)
                if (registry->get_component<Key>(content))
                    ++keys;

            if (keys == 0) useable->toggle = false;

            // If player has a key and is interacting with the door, open the door and consume one key
            if (keys > 0 && useable->toggle)
            {
                lock->open = true;
                for (size_t i = 0; i < contents->contents.size(); ++i)
                {
                    Entity c = contents->contents[i];
                    if (registry->get_component<Key>(c))
                    {
                        contents->contents.erase(contents->contents.begin() + static_cast<ptrdiff_t>(i));
                        break;
                    }
                }
            }

            // Set locked-door appearance and ensure collision is enabled
            if (sp) sp->texture_id = std::byte(8);
            Collision* col = registry->get_component<Collision>(entity);
            if (col) col->enabled = true;

            continue;
        }

        // Door is unlocked: toggle open/close state based on useable->toggle
        Collision* col = registry->get_component<Collision>(entity);
        if (!useable->toggle)
        {
            if (sp) sp->texture_id = std::byte(9);
            if (col) col->enabled = true;
        }
        else
        {
            if (sp) sp->texture_id = std::byte(10);
            if (col) col->enabled = false;
        }
    }
}

void teleport_event()
{
    // Teleports
    for (auto& pair : registry->get_all<Teleport>())
    {
        Entity entity = pair.first;
        Sprite* sp = registry->get_component<Sprite>(entity);
        if (!sp || sp->texture_id == std::byte(35)) continue;

        Entity player = registry->get_all<Player>().empty() ? 0 : registry->get_all<Player>().begin()->first;
        if (player == 0) continue;

        Transform* transform = registry->get_component<Transform>(entity);
        Transform* pt = registry->get_component<Transform>(player);
        Teleport* teleport = registry->get_component<Teleport>(entity);
        if (!transform || !pt || !teleport) continue;

        // Ignore inactive teleports
        if (!teleport->active) continue;

        float px = pt->position.x;
        float py = pt->position.y;
        float pw = pt->size.x;
        float ph = pt->size.y;

        float tx = transform->position.x;
        float ty = transform->position.y;
        float tw = transform->size.x;
        float th = transform->size.y;

        // Teleport player to next area if in bounding box
        if (px + (pw / 2) <= tx + tw && px + (pw / 2) >= tx &&
            py + (ph / 2) <= ty + th && py + (ph / 2) >= ty)
        {
            pt->position = teleport->next_position;
            util::map::map_to_load = teleport->map_to_load;
            util::map::map_loaded = false;
        }
    }
}