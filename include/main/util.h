#pragma once

#include <mutex>

inline constexpr int NANOSECOND = 1'000'000'000;
extern int frame_rate;

struct Point
{
	float x = 0.0f, y = 0.0f;

	bool operator==(const Point& other) const 
	{
		return x == other.x && y == other.y;
	}

	Point operator+(const Point& other) const
	{
		return { x + other.x, y + other.y };
	}
};

namespace util 
{
	namespace key_manager
	{
		inline bool w = false, a = false, s = false, d = false, space = false, paused = false;
	};

	namespace mouse_manager
	{
		inline double x = 0, y = 0;
		inline bool left = false, right = false;
	};

	namespace map
	{
		inline bool map_loaded = false;
		inline int map_to_load = 1;
	}

	namespace time
	{
		inline bool sun_rise = true;
		inline float day = 0.0f;
	}

	namespace settings
	{
		inline float brightness = 0.925f;
		inline int v_sync = 1;
		inline bool fullscreen = false;
	}

	namespace thread 
	{
		inline bool physics_done = false, running = true;
		inline std::thread physics_thread{};
	}
}

namespace std
{
	template<>
	struct hash<Point> 
	{
		std::size_t operator()(const Point& p) const noexcept 
		{
			std::size_t h1 = std::hash<float>{}(p.x);
			std::size_t h2 = std::hash<float>{}(p.y);
			return h1 ^ (h2 << 1);
		}
	};
}