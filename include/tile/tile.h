#pragma once
#include <unordered_map>
#include "../main/util.h"
#include "../main/texture.h"

class Tile
{
private:
	Point m_pos;
	const bool m_collisions = false;
	const std::byte m_id = std::byte(5);
public:

	static inline std::unordered_map<Point, Tile> s_tiles{};
	static inline constexpr float s_size = 0.2f;

	Tile(Point pos, std::byte id)
		: m_pos(pos), m_id(id)
	{
		s_tiles.emplace(m_pos, *this);
	}

	Tile(float x, float y, std::byte id)
		: m_pos(Point{ x, y }), m_id(id)
	{
		s_tiles.emplace(m_pos, *this);
	}

	constexpr Point get_position() const { return m_pos; }
	constexpr std::byte get_id() const { return m_id; }
	constexpr bool has_collisions() const { return m_collisions; }

	constexpr void set_position(Point pos) { m_pos = pos; }
	constexpr void set_position_x(float x) { m_pos.x = x; }
	constexpr void set_position_y(float y) { m_pos.y = y; }

	virtual constexpr ~Tile() = default;

};