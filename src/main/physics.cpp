#include <thread>
#include <chrono>
#include "physics.h"

bool util::thread::physics_done = false;
bool util::thread::running = true;
float generosity = 0.03f; // How much collision boxes are radially offset for fairness
std::thread util::thread::physics_thread(run);
std::mutex util::thread::mtx;
std::condition_variable util::thread::cv;

void start()
{
	util::thread::physics_thread.detach();
}

void run()
{
	std::chrono::nanoseconds frame_duration(1'000'000'000 / frame_rate);
	float delta_time = 0.0f;

	auto last_time = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds target_time(frame_duration);

	while (util::thread::running)
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto elapsed_time = now - last_time;

		if (elapsed_time >= target_time)
		{
			delta_time = std::chrono::duration<float>(now - last_time).count();
			last_time = now;
			physics_update(delta_time);
			{
				std::lock_guard<std::mutex> lock(util::thread::mtx);
				util::thread::physics_done = true;
			}
			util::thread::cv.notify_one();
		}
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void physics_update(const float delta_time)
{
    float size = Entity::s_size;

    for (Entity* entity : Entity::s_entities)
    {
        float speed = entity->speed;
        Point velocity = entity->get_velocity();
        Point pos = entity->get_position();

        if (Player* ptr = dynamic_cast<Player*>(entity))
        {
            using namespace util::key_manager;
            if ((w && a) || (w && d) || (s && a) || (s && d))
                speed /= std::sqrt(2.0f);

            if (w) velocity.y = speed * delta_time;
            if (s) velocity.y = -speed * delta_time;
            if (!w && !s) velocity.y = 0;

            if (a) velocity.x = -speed * delta_time;
            if (d) velocity.x = speed * delta_time;
            if (!a && !d) velocity.x = 0;

            entity->set_velocity(velocity);
        }

        pos.x += velocity.x;

        for (auto& pair : Tile::s_tiles)
        {
            Tile tile = pair.second;

            byte id = tile.get_id();
            if (id == 0 || id == 1) continue;

            float tx = tile.get_position().x;
            float ty = tile.get_position().y;
            float tw = Tile::s_size;
            float th = Tile::s_size;

            if (pos.x + generosity < tx + tw && pos.x + size - generosity > tx &&
                pos.y + generosity < ty + th && pos.y + size - generosity > ty)
            {
                if (velocity.x > 0) pos.x = tx - size + generosity - 0.0001f;
                else if (velocity.x < 0) pos.x = tx + tw - generosity + 0.0001f;
                velocity.x = 0;
            }
        }

        pos.y += velocity.y;

        for (auto& pair : Tile::s_tiles)
        {
            Tile tile = pair.second;

            byte id = tile.get_id();
            if (id == 0 || id == 1) continue;

            float tx = tile.get_position().x;
            float ty = tile.get_position().y;
            float tw = Tile::s_size;
            float th = Tile::s_size;

            if (pos.x + generosity < tx + tw && pos.x + size - generosity > tx &&
                pos.y + generosity < ty + th && pos.y + size - generosity > ty)
            {
                if (velocity.y > 0) pos.y = ty - size + generosity - 0.0001f;
                else if (velocity.y < 0) pos.y = ty + th - generosity + 0.0001f;
                velocity.y = 0;
            }
        }

        entity->set_velocity(velocity);
        entity->set_position(pos);
    }
}