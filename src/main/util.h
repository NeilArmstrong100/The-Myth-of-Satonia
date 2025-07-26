#pragma once
#include <mutex>

typedef unsigned char byte;

struct Point
{
	float x, y;

	bool operator==(const Point& other) const 
	{
		return x == other.x && y == other.y;
	}

	Point operator+(const Point& other) const
	{
		return { x + other.x, y + other.y };
	}
};

extern int frame_rate;

namespace util 
{
	namespace key_manager 
	{
		extern bool w, a, s, d, space;
	}

	namespace thread 
	{
		extern bool physics_done, running;
		extern std::thread physics_thread;
		extern std::mutex mtx;
		extern std::condition_variable cv;
	}

	extern Point snap_grid(const Point, const float);
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